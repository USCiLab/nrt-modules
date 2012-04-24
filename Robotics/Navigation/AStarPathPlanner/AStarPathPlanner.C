#include "AStarPathPlanner.H"
#include <nrt/ImageProc/Drawing/Geometry.H>

using namespace nrt;
using namespace astarpathplanner;

// ######################################################################
class PriorityComparator
{
  public:
    PriorityComparator(std::vector<double> const & costs) : itsCosts(costs) { }

    bool operator() (int const & lhs, int const & rhs) const
    {
      return itsCosts[lhs] < itsCosts[rhs];
    }

  private:
    std::vector<double> const & itsCosts;
};

// ######################################################################
AStarPathPlannerModule::AStarPathPlannerModule(std::string const& instanceName) :
  Module(instanceName),
  itsNextTransformParam(NextTransformParam, this),
  itsFromTransformParam(FromTransformParam, this),
  itsToTransformParam(ToTransformParam, this),
  itsSegmentLengthParam(SegmentLengthParam, this),
  itsUpdateRateParam(UpdateRateParam, this),
  itsShowDebugParam(ShowDebugParam, this, &AStarPathPlannerModule::debugParamCallback)
{
  //
}

// ######################################################################
std::vector<Point2D<int>> AStarPathPlannerModule::AStar(Image<PixGray<byte>> const mapInput, Point2D<int> start, Point2D<int> goal)
{
  Image<PixGray<byte>, UniqueAccess> map(mapInput);
  int const w = map.width();
  int const h = map.height();

  bool lefWallOk = false;
  bool rigWallOk = false;
  bool topWallOk = false;
  bool botWallOk = false;
  if (goal.x() < 0 || goal.x() >= w ||
      goal.y() < 0 || goal.y() >= h)
  {
    Line<int> startGoal = map.bounds().clip(Line<int>(start, goal));
    Point2D<int> edgePoint = startGoal.p1() == start ? startGoal.p2() : startGoal.p1();

    lefWallOk = edgePoint.x() == 0;
    rigWallOk = edgePoint.x() == w-1;
    topWallOk = edgePoint.y() == 0;
    botWallOk = edgePoint.y() == h-1;
  }

  auto pos2ind = [&](Point2D<int> pos)
  {
    return pos.y() * w + pos.x();
  };

  auto ind2pos = [&](int ind)
  {
    int y = ind / w;
    int x = ind - y*w;
    return Point2D<int>(x, y);
  };

  auto heuristic = [&](Point2D<int> first, Point2D<int> second)
  {
    return first.distanceTo(second);
  };

  std::vector<Point2D<int>> neighbors = {
    Point2D<int>( 0,  1),
    Point2D<int>( 0, -1),
    Point2D<int>( 1,  0),
    Point2D<int>( 1,  1),
    Point2D<int>( 1, -1),
    Point2D<int>(-1,  0),
    Point2D<int>(-1,  1),
    Point2D<int>(-1, -1),
  };

  std::vector<int> camefrom(map.size()+1, -1);
  std::vector<double> gscore(map.size()+1, -1);
  std::vector<double> hscore(map.size()+1, -1);
  std::vector<double> fscore(map.size()+1, -1);

  gscore[pos2ind(start)] = 0;
  hscore[pos2ind(start)] = heuristic(start, goal);
  fscore[pos2ind(start)] = gscore[pos2ind(start)] + hscore[pos2ind(start)];

  std::set<int> closedset;
  std::set<int, PriorityComparator> openset( (PriorityComparator(fscore)) );
  openset.insert(pos2ind(start));

  while (!openset.empty())
  {
    int current = *openset.begin();
    if (current == pos2ind(goal) || current == map.size())
    {
      std::vector<Point2D<int>> path;
      while (current != pos2ind(start))
      {
        path.push_back(ind2pos(current));
        current = camefrom[current];
      }
      std::reverse(path.begin(), path.end());
      return path;
    }

    openset.erase(openset.begin());
    closedset.insert(current);
    for (Point2D<int> const & neighboroffset : neighbors)
    {
      Point2D<int> neighborPos = ind2pos(current) + neighboroffset;
      int neighbor = pos2ind(neighborPos);

      if (neighborPos.x() < 0 || neighborPos.x() >= w  || neighborPos.y() < 0 || neighborPos.y() >= h )
      {
        if (neighborPos.x() < 0  && lefWallOk || 
            neighborPos.x() >= w && rigWallOk ||
            neighborPos.y() < 0  && topWallOk ||
            neighborPos.y() >= h && botWallOk) 
          neighbor = map.size();
        else continue;
      }

      if(map(neighbor).val() == 255)
        continue;

      if(closedset.count(neighbor))
        continue;

      double g = gscore[current] + neighborPos.distanceTo(goal);

      if (!openset.count(neighbor))
      {
        camefrom[neighbor] = current;
        gscore[neighbor] = g;
        hscore[neighbor] = heuristic(neighborPos, goal);
        fscore[neighbor] = g + hscore[neighbor];
        openset.insert(neighbor);
      }
      else if (g < gscore[neighbor])
      {
        camefrom[neighbor] = current;
        gscore[neighbor] = g;
        fscore[neighbor] = g + hscore[neighbor];
      }
    }
  }
  return std::vector<Point2D<int>>();
}

// ######################################################################
nrt::Transform3D AStarPathPlannerModule::lookupTransform(std::string from, std::string to)
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
void AStarPathPlannerModule::debugParamCallback(bool const & debug)
{
  std::lock_guard<std::mutex> _(itsMtx);
  
  if (debug && !itsDisplay)
  {
    itsDisplay.reset(new nrt::DisplayImageSink);
    addSubComponent(itsDisplay);
  }
  else if (!debug && itsDisplay)
  {
    removeSubComponent(itsDisplay);
    itsDisplay.reset();
  }
}

// ######################################################################
void AStarPathPlannerModule::run()
{
  while (running())
  {
    auto endTime = nrt::now() + std::chrono::milliseconds((int)std::round(1000.0/itsUpdateRateParam.getVal()));
    if (auto mapres = check<astarpathplanner::OccupancyMap>(nrt::MessageCheckerPolicy::Unseen))
    {
      nrt::real pixelsPerMeter;
      //if (auto ppmres = check<astarpathplanner::PixelsPerMeter>(nrt::MessageCheckerPolicy::Any))
      {
        try
        {
          pixelsPerMeter = 10;// ppmres.get()->value;

          Image<PixGray<byte>> map = mapres.get()->img.convertTo<PixGray<byte>>();
          nrt::Transform3D goalTransform = lookupTransform(itsFromTransformParam.getVal(), itsToTransformParam.getVal());
          Eigen::Vector3d goalVector = goalTransform * Eigen::Vector3d(0, 0, 0);

          Point2D<int> start(map.width()/2, map.height()/2);
          Point2D<int> goal(goalVector.x(), goalVector.y());
          goal *= pixelsPerMeter;
          goal += start;
          NRT_INFO("I think my Goal is at position " << goal << " relative to the robot");

          std::vector<Point2D<int>> path = AStar(map, start, goal);
          if (path.size())
          {
            int n = pixelsPerMeter * itsSegmentLengthParam.getVal();
            if (n >= path.size())
              n = path.size()-1;

            // post the transform
            nrt::Transform3D transform(Eigen::Translation3d(path[n].x(), path[n].y(), 0.0));
            std::unique_ptr<nrt::TransformMessage> transformMsg(new nrt::TransformMessage(nrt::now(), itsFromTransformParam.getVal(), itsNextTransformParam.getVal(), transform));
            post<NextTransform>(transformMsg);
            
            // post the path itself
            std::unique_ptr<nrt::PathMessage> pathMsg(new nrt::PathMessage(path));
            post<ComputedPath>(pathMsg);

            {
              std::lock_guard<std::mutex> _(itsMtx);
              if (itsDisplay)
              {
                Image<PixRGB<byte>> pathImage(map);
                double i = 0;
                for (Point2D<int> const & p : path)
                {
                  drawDisk(pathImage, Circle<int>(p, 2), PixHSV<double>(255*i/path.size(), 255, 128));
                  drawLine(pathImage, Line<int>(start, goal), PixRGBA<byte>(255, 0, 0, 128));
                  i++;
                }
                itsDisplay->out(GenericImage(pathImage));
              }
            }
          }
        }
        catch (std::runtime_error &e)
        {
          NRT_WARNING("Exception: " << e.what());
          continue;
        }
      }
      //else
      //{
      //  NRT_WARNING("No PixelsPerMeter conversion found!");
      //}
    }
    else
    {
      NRT_WARNING("No occupancy grid found!");
    }
    
    if (nrt::now() > endTime)
      NRT_WARNING("Cannot maintain update rate!");

    std::this_thread::sleep_until(endTime);
  }
}

NRT_REGISTER_MODULE(AStarPathPlannerModule);
