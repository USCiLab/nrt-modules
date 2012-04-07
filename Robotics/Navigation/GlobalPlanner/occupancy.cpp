#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/graph/astar_search.hpp>
#include <boost/graph/grid_graph.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/random.hpp>
#include <boost/random.hpp>
#include <boost/graph/graphviz.hpp>
#include <nrt/ImageProc/IO/ImageSource/ImageSources.H>
#include <nrt/ImageProc/IO/ImageSource/ImageReaders/ImageReader.H>
#include <nrt/ImageProc/IO/ImageSink/ImageSinks.H>
#include <nrt/ImageProc/Filtering/Morphology.H>
#include <nrt/ImageProc/Drawing/Geometry.H>
#include <nrt/Core/Model/Manager.H>

using namespace nrt;
using namespace boost;

typedef boost::grid_graph<2> grid;
typedef boost::graph_traits<grid>::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits<grid>::vertices_size_type vertices_size_type;

struct vertex_hash : std::unary_function<vertex_descriptor, std::size_t>
{
  std::size_t operator()(vertex_descriptor const& u) const {
    std::size_t seed = 0;
    boost::hash_combine(seed, u[0]);
    boost::hash_combine(seed, u[1]);
    return seed;
  }
};

typedef boost::unordered_set<vertex_descriptor, vertex_hash> vertex_set;
typedef boost::vertex_subset_complement_filter<grid, vertex_set>::type filtered_grid;

class euclidean_heuristic:
  public boost::astar_heuristic<grid, float>
{
  public:
    euclidean_heuristic(vertex_descriptor goal):m_goal(goal) {};

    float operator()(vertex_descriptor v) {
      return sqrt(pow(m_goal[0] - v[0], 2) + pow(m_goal[1] - v[1], 2));
    }

  private:
    vertex_descriptor m_goal;
};

struct found_goal {};

struct astar_goal_visitor:public boost::default_astar_visitor
{
  astar_goal_visitor(vertex_descriptor goal):m_goal(goal) {};

  void examine_vertex(vertex_descriptor u, const filtered_grid&) {
    if (u == m_goal)
      throw found_goal();
  }

  private:
  vertex_descriptor m_goal;
};

int main(int argc, const char** argv)
{
  Manager mgr(argc, argv);

  std::shared_ptr<ImageSink> mySink(new ImageSink);
  mgr.addSubComponent(mySink);

  Parameter<std::string> fileParam(ParameterDef<std::string>("file", "The occupancy grid image", ""), &mgr);
  Parameter<int> dilateParam(ParameterDef<int>("dilation", "The amount to dilate", 3), &mgr);

  mgr.launch();

  Image<PixGray<byte>> inputImage = readImage(fileParam.getVal()).convertTo<PixGray<byte>>();
  Image<PixGray<byte>> image = nrt::dilate(inputImage, dilateParam.getVal());

  // make a new grid graph that's WxH size
  // go through each node
  // if that node has a value >= 128 in the image, remove it from the graph
  // now do a* on the resulting graph
  //
  //

  boost::array<std::size_t, 2> lengths = { { image.width(), image.height() } };
  grid occupancyMap(lengths);

  mt19937 gen(time(0));
  vertex_descriptor start = boost::vertex(0, occupancyMap); // boost::random_vertex(occupancyMap, gen);
  vertex_descriptor goal  = boost::vertex(image.size()-1, occupancyMap); //boost::random_vertex(occupancyMap, gen);
  vertex_set walls;

  byte const * const imageBegin = image.pod_begin();
  for (vertices_size_type v_index = 0; v_index < boost::num_vertices(occupancyMap); v_index++)
  {
    if (imageBegin[v_index] == 255) 
    {
      // add to vertex_set
      walls.insert(boost::vertex(v_index, occupancyMap));
    }
  }

  filtered_grid costMap = boost::make_vertex_subset_complement_filter(occupancyMap, walls);

  boost::static_property_map<float> weight(1);
  typedef boost::unordered_map<vertex_descriptor, vertex_descriptor, vertex_hash> pred_map;
  pred_map predecessor;

  boost::associative_property_map<pred_map> pred_pmap(predecessor);

  typedef boost::unordered_map<vertex_descriptor, float, vertex_hash> dist_map;

  dist_map distance;
  boost::associative_property_map<dist_map> dist_pmap(distance);

  euclidean_heuristic heuristic(goal);
  astar_goal_visitor visitor(goal);

  vertex_set solution;
  float solution_length;

  Image<PixRGB<byte>> solutionImage(inputImage);

  bool solutionFound = false;
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
    solutionFound = true;
    solution_length = distance[goal];
    float i = 0;
    for (vertex_descriptor u = goal; u != start; u = predecessor[u])
    {
      solution.insert(u);
      std::size_t index = get(boost::vertex_index, occupancyMap, u);
      //solutionImage(index) = PixRGB<byte>(PixHSV<float>(255*i/solution_length, 255, 128));
      int y = index / image.width();
      int x = index - y*image.width();
      drawDisk(solutionImage, Circle<int>(Point2D<int>(x,y), 3), PixHSV<float>(255*i/solution_length, 255, 128));
      i+=1.0;
    }
    solution.insert(start);


    std::cout << "Solution found! Length = " << solution_length << std::endl;
  }

  while (solutionFound)
  {
    mySink->out(GenericImage(solutionImage), "Occupancy Map");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  return 1;
}
