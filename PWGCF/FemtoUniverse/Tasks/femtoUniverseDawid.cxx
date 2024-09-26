#include <Framework/Configurable.h>
#include <Framework/Expressions.h>
#include <Framework/HistogramRegistry.h>
#include <Framework/HistogramSpec.h>
#include <iostream>
#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct femtoUniverseDawid {
  Configurable<int> nBinsPt{"nBinsPt", 100, "N bins pt"};

  HistogramRegistry histos{"histos", {}};

  // define filter
  Filter ptFilter{aod::track::pt > 1};

  // cache for slices
  SliceCache cache;
  Partition<aod::Tracks> leftTracks{aod::track::eta < 0};
  Partition<aod::Tracks> rightTracks{aod::track::eta >= 0};

  void init()
  {
    std::cout << "init\n";
    histos.add("hPt", "pt;x;y", kTH1F, {{nBinsPt, 0.0, 5.0}});
    histos.add("hEvCount", "hEvCount", kTH1F, {{1, 0.0, 1.0}});
  };

  void process(aod::Collision const& collision, soa::Filtered<aod::Tracks> const& tracks)
  {
    std::cout << "process\n";

    // assign partition to specific collision == slicing
    auto leftTracksCollision = leftTracks->sliceByCached(aod::track::collisionId, collision.globalIndex(), cache);
    auto rightTracksCollision = rightTracks->sliceByCached(aod::track::collisionId, collision.globalIndex(), cache);

    histos.fill(HIST("hEvCount"), 0.5);
    for (const auto& track : leftTracksCollision) {
      std::cout << track.pt() << '\n';
      histos.fill(HIST("hPt"), track.pt());
    }
  };
  PROCESS_SWITCH(femtoUniverseDawid, process, "Do process", false);
};

WorkflowSpec defineDataProcessing(ConfigContext const& configContext)
{
  WorkflowSpec workflow{
    adaptAnalysisTask<femtoUniverseDawid>(configContext),
  };
  return workflow;
}
