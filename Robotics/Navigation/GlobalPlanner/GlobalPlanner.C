#include "GlobalPlanner.H"
#include <nrt/Core/Image/Image.H>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/graph/astar_search.hpp>
#include <boost/graph/grid_graph.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/random.hpp>
#include <boost/random.hpp>

using namespace nrt;
using namespace globalplanner;

// ######################################################################
// some typedefs for clarity
typedef boost::adjacency_list<boost::vecS, boost::listS, boost::undirectedS, std::array<float,2>, float> Graph;
typedef boost::grid_graph<2> GridGraph;
typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;
typedef boost::graph_traits<Graph>::vertices_size_type VerticesSizeType;
typedef typename Graph::edge_property_type Weight;


struct vertex_hash : std::unary_function<VertexDescriptor, std::size_t>
{
  std::size_t operator()(VertexDescriptor const& u) const {
    std::size_t seed = 0;
    boost::hash_combine(seed, u[0]);
    boost::hash_combine(seed, u[1]);
    return seed;
  }
};

typedef boost::unordered_set<VertexDescriptor, vertex_hash> VertexSet;
typedef boost::vertex_subset_complement_filter<Graph, VertexSet>::type FilteredGraph;
typedef boost::unordered_map<VertexDescriptor, VertexDescriptor, vertex_hash> PredMap;
typedef boost::unordered_map<VertexDescriptor, float, vertex_hash> DistMap;


class euclidean_heuristic:
  public boost::astar_heuristic<Graph, float>
{
  public:
    euclidean_heuristic(VertexDescriptor goal):m_goal(goal) {};

    float operator()(VertexDescriptor v) {
      return sqrt(pow(m_goal[0] - v[0], 2) + pow(m_goal[1] - v[1], 2));
    }

  private:
    VertexDescriptor m_goal;
};

struct found_goal {};

struct astar_goal_visitor:public boost::default_astar_visitor
{
  astar_goal_visitor(VertexDescriptor goal):m_goal(goal) {};

  void examine_vertex(VertexDescriptor u, const FilteredGraph&) {
    if (u == m_goal)
      throw found_goal();
  }

  private:
  VertexDescriptor m_goal;
};
// ######################################################################
// ######################################################################
GlobalPlannerModule::GlobalPlannerModule(std::string const& instanceName) :
  Module(instanceName),
  itsNextTransformParam(NextTransformParam, this),
  itsWorldTransformParam(WorldTransformParam, this),
  itsRobotTransformParam(RobotTransformParam, this),
  itsGoalTransformParam(GoalTransformParam, this),
  itsIntervalParam(IntervalParam, this)
{
  //
}

// ######################################################################
nrt::Transform3D GlobalPlannerModule::lookupTransform(std::string from, std::string to)
{
  auto lookupMessage = nrt::make_unique(new TransformLookupMessage(nrt::now(), from, to));
  MessagePosterResults<TransformLookup> results = post<TransformLookup>(lookupMessage);

  if(results.size() == 0)
  {
    throw std::runtime_error("No TransformManagers detected");
  }
  if(results.size() > 1)
  { 
    throw std::runtime_error("Multiple TransformManagers detected!");
  }

  TransformMessage::const_ptr transform = results.get();
  return transform->transform;
}

// ######################################################################
void GlobalPlannerModule::onMessage(globalplanner::OccupancyMap map)
{
  auto image = map->img.convertTo<PixGray<byte>>();
  try
  {
    Eigen::Vector3d robot2bread = lookupTransform(itsRobotTransformParam.getVal(), itsNextTransformParam.getVal()) * Eigen::Vector3d(0, 0, 0);
    if ( sqrt(robot2bread.x()*robot2bread.x() + robot2bread.y()*robot2bread.y() + robot2bread.z()*robot2bread.z()) > 0.10) 
    {
      int pixelw = image.width();
      int pixelh = image.height();
      float mapw = pixelw * itsPixelsPerMeter;
      float maph = pixelh * itsPixelsPerMeter;

      boost::array<std::size_t, 2> lengths = {{pixelw, pixelh}};
      GridGraph gridGraph(lengths);
      Graph occupancyGraph;
      boost::copy_graph(gridGraph, occupancyGraph);

      // 1) Get robot->goal transform
      nrt::Transform3D robot2goal = lookupTransform(itsRobotTransformParam.getVal(), itsNextTransformParam.getVal());
      Eigen::Vector3d goal = robot2goal * Eigen::Vector3d(0,0,0);

      Point2D<float> goal2D(goal.x(), goal.y());

      if(goal.x() < -mapw/2 || goal.x() > mapw/2 || goal.y() < -maph/2 || goal.y() > maph/2)
      {
        // The goal is inside our map
      }
      else
      {
        // The goal is outside of our map
        VertexDescriptor goal = boost::add_vertex(occupancyGraph);
        for(int x = 0; x<pixelw; x++)
        {
          float dist1 = Point2D<float>(x*itsPixelsPerMeter, 0).manhattanDistanceTo(goal2D);
          float dist2 = Point2D<float>(x*itsPixelsPerMeter, maph).manhattanDistanceTo(goal2D);

          add_edge( boost::vertex(x, occupancyGraph), goal, Weight(dist1), occupancyGraph);
          add_edge( boost::vertex(pixelh*pixelw + x, occupancyGraph), goal, Weight(dist2), occupancyGraph);
        }
        for(int y = 0; y<pixelh; y++)
        {
          float dist1 = Point2D<float>(0,    y*itsPixelsPerMeter).manhattanDistanceTo(goal2D);
          float dist2 = Point2D<float>(mapw, y*itsPixelsPerMeter).manhattanDistanceTo(goal2D);

          add_edge( boost::vertex(y*pixelw, occupancyGraph), goal, Weight(dist1), occupancyGraph);
          add_edge( boost::vertex(y*pixelw + pixelw, occupancyGraph), goal, Weight(dist2), occupancyGraph);
        }
      }

      VertexSet obstacles;
      byte const * const imageBegin = image.pod_begin();
      for (VerticesSizeType v_index = 0; v_index < boost::num_vertices(occupancyGraph); v_index++)
      {
        if (imageBegin[v_index] == 255)
        {
          obstacles.insert(boost::vertex(v_index, occupancyGraph));
        }
      }

      FilteredGraph costMap = boost::make_vertex_subset_complement_filter(occupancyGraph, obstacles);
      boost::property_map<GridGraph, boost::edge_weight_t>::type weightmap = get(edge_weight, occupancyGraph);

      PredMap predecessor;
      boost::associative_property_map<PredMap> pred_pmap(predecessor);

      DistMap distance;
      boost::associative_property_map<DistMap> dist_pmap(distance);

      euclidean_heuristic heuristic(goal);
      astar_goal_visitor visitor(goal);

      VertexSet solution;
      float solution_length;

      bool solution_found = false;
      try
      {
        boost::astar_search(costMap, start, heuristic,
          boost::weight_map(weight).
          predecessor_map(pred_pmap).
          distance_map(dist_pmap).
          visitor(visitor)
        );
      }
      catch (found_goal fg)
      {
        solution_found = true;
        solution_length = distance[goal];
        for (VertexDescriptor u = goal; u != start; u = predecessor[u])
        {
          solution.insert(u);
        }
        solution.insert(start);
      }

      // post a trasnform at solution[itsIntervalParam.getVal()*itsPixelsPerMeter]
      VertexDescriptor nextBreadcrumb = solution[itsIntervalParam.getVal() * itsPixelsPerMeter];
      std::size_t index = get(boost::vertex_index, occupancyGraph, nextBreadcrumb);
      int y = index / mapw;
      int x = index - y*mapw;
      itsLastBreadcrumb = Eigen::Translation<double, 3>(x, y, 0);
      // post itsLastBreadCrumb on the NextTransform port
      std::unique_ptr<nrt::TransformMessage> msg(
          new nrt::TransformMessage(nrt::now(), itsRobotTransformParam.getVal(), itsNextTransformParam.getVal(), itsLastBreadcrumb));
      post<NextTransform>(msg);
    }
  }
  catch (std::runtime_error e)
  {
    NRT_WARNING(e.what());
  }
}
