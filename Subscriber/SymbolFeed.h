#pragma once
#include "SymbolFeed.h"

//#include "Decoder.fwd.h"

#include "Handler.h"
#include "Receiver.h"

#include "sbe/MDIncrementalRefreshBook32.h"
#include "sbe/MDIncrementalRefreshDailyStatistics33.h"
#include "sbe/MDIncrementalRefreshLimitsBanding34.h"
#include "sbe/MDIncrementalRefreshOrderBook43.h"
#include "sbe/MDIncrementalRefreshSessionStatistics35.h"
#include "sbe/MDIncrementalRefreshTrade36.h"
#include "sbe/MDIncrementalRefreshTradeSummary42.h"
#include "sbe/MDIncrementalRefreshVolume37.h"
#include "sbe/MessageHeader.h"
#include "sbe/SnapshotFullRefresh38.h"
#include "sbe/SnapshotFullRefreshOrderBook44.h"

class SymbolFeed {

  bool recoverymode_ = false;
  int seqnum_ = 0;
  uint64_t securityid_;

  void StartRecovery();
  void StopRecovery();
  void HandleBidEntry(MDUpdateAction::Value action, int level, float price,
                      int volume);
  void HandleAskEntry(MDUpdateAction::Value action, int level, float price,
                      int volume);
  Handler &handler_; // = nullptr;
                     //  Decoder &decoder_; // = nullptr;

  OrderBook book_;

  Receiver incrementalA_;
  Receiver incrementalB_;
  Receiver snapshotA_;
  Receiver snapshotB_;

public:
  // SymbolFeed(uint64_t, Handler &, Decoder &);
  SymbolFeed(uint64_t, Handler &);
  //  SymbolFeed() = default;
  ~SymbolFeed();
  void OnMDIncrementalRefreshBook32(MDIncrementalRefreshBook32 &);
  void OnMDIncrementalRefreshDailyStatistics33(
      MDIncrementalRefreshDailyStatistics33 &);
  void
  OnMDIncrementalRefreshLimitsBanding34(MDIncrementalRefreshLimitsBanding34 &);
  void OnMDIncrementalRefreshSessionStatistics35(
      MDIncrementalRefreshSessionStatistics35 &);
  void OnMDIncrementalRefreshTrade36(MDIncrementalRefreshTrade36 &);
  void OnMDIncrementalRefreshVolume37(MDIncrementalRefreshVolume37 &);
  void
  OnMDIncrementalRefreshTradeSummary42(MDIncrementalRefreshTradeSummary42 &);
  void OnMDIncrementalRefreshOrderBook43(MDIncrementalRefreshOrderBook43 &);
  void OnMDSnapshotFullRefresh38(SnapshotFullRefresh38 &);
  void OnMDSnapshotFullRefreshOrderBook44(SnapshotFullRefreshOrderBook44 &);
};
