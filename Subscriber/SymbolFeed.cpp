#include "SymbolFeed.h"

#include <cmath>

SymbolFeed::SymbolFeed(uint64_t securityid, Handler &handler, Decoder
 &decoder) : securityid_(securityid), handler_(handler), decoder_(decoder) {

  using std::placeholders::_1;

  decoder.RegisterCallbacks(
   std::bind(&SymbolFeed::OnMDIncrementalRefreshBook32&, this, _1),
   std::bind(&SymbolFeed::OnMDIncrementalRefreshDailyStatistics33&, this, _1),
   std::bind(&SymbolFeed::OnMDIncrementalRefreshLimitsBanding34&, this, _1),
   std::bind(&SymbolFeed::OnMDIncrementalRefreshSessionStatistics35&, this, _1),
   std::bind(&SymbolFeed::OnMDIncrementalRefreshTrade36&, this, _1),
   std::bind(&SymbolFeed::OnMDIncrementalRefreshVolume37&, this, _1),
   std::bind(&SymbolFeed::OnMDIncrementalRefreshTradeSummary42&, this, _1),
   std::bind(&SymbolFeed::OnMDIncrementalRefreshOrderBook43&, this, _1),
   std::bind(&SymbolFeed::OnMDSnapshotFullRefresh38&, this, _1),
   std::bind(&SymbolFeed::OnMDSnapshotFullRefreshOrderBook44&, this, _1)
  );

  incrementalA_.join();
  incrementalB_.join();
  StartRecovery();
}

SymbolFeed::~SymbolFeed() {
  incrementalA_.leave();
  incrementalB_.leave();
  snapshotA_.leave();
  snapshotB_.leave();
}

void SymbolFeed::StartRecovery() {

  std::cout << "Starting Recovery";

  if (!recoverymode_) {
    recoverymode_ = true;
    book_.Clear();
    seqnum_ = 0; // ensures subsequent incrementals are ignored until snapshot
                 // alignment
    snapshotA_.join();
    snapshotB_.join();
  }
}

void SymbolFeed::StopRecovery() {

  std::cout << "Stopping Recovery";

  if (recoverymode_) {
    recoverymode_ = false;
    snapshotA_.leave();
    snapshotB_.leave();
  }
}

void SymbolFeed::OnMDSnapshotFullRefresh38(SnapshotFullRefresh38 &refresh) {

  if (!recoverymode_)
    return;

  if (refresh.securityID() != securityid_)
    return;

  auto &entry = refresh.noMDEntries();

  book_.Clear();
  seqnum_ = refresh.lastMsgSeqNumProcessed(); // fast forward seqnum for
                                              // incremental feed alignment

  while (entry.hasNext()) {
    entry.next();

    int level = entry.mDPriceLevel();
    float price = entry.mDEntryPx().mantissa() *
                  std::pow(10, entry.mDEntryPx().exponent());
    int volume = entry.mDEntrySize();

    if (entry.mDEntryType() == MDEntryType::Bid) {
      book_.AddBid(level, price, volume);
    } else if (entry.mDEntryType() == MDEntryType::Offer) {
      book_.AddAsk(level, price, volume);
    }
  }
  handler_.OnQuote(book_);
}

void SymbolFeed::HandleAskEntry(MDUpdateAction::Value action, int level,
                                float price, int volume) {
  switch (action) {
  case MDUpdateAction::New:
    book_.AddAsk(level, price, volume);
    break;
  case MDUpdateAction::Change:
    book_.UpdateAsk(level, price, volume);
    break;
  case MDUpdateAction::Delete:
    book_.DeleteAsk(level, price);
    break;
  case MDUpdateAction::DeleteFrom:
    book_.DeleteAskFrom(level);
    break;
  case MDUpdateAction::DeleteThru:
    book_.DeleteAskThru(level);
    break;
  }
}
void SymbolFeed::HandleBidEntry(MDUpdateAction::Value action, int level,
                                float price, int volume) {
  switch (action) {
  case MDUpdateAction::New:
    book_.AddBid(level, price, volume);
    break;
  case MDUpdateAction::Change:
    book_.UpdateBid(level, price, volume);
    break;
  case MDUpdateAction::Delete:
    book_.DeleteBid(level, price);
    break;
  case MDUpdateAction::DeleteFrom:
    book_.DeleteBidFrom(level);
    break;
  case MDUpdateAction::DeleteThru:
    book_.DeleteBidThru(level);
    break;
  }
}

void SymbolFeed::OnMDIncrementalRefreshBook32(
    MDIncrementalRefreshBook32 &refresh) {

  auto &entry = refresh.noMDEntries();

  while (entry.hasNext()) {

    entry.next();

    if (entry.securityID() != securityid_)
      continue;
    if (entry.rptSeq() != seqnum_ + 1)
      continue;
    if (entry.rptSeq() > seqnum_ + 1) {
      StartRecovery();
      break;
    }

    if (recoverymode_)
      StopRecovery();

    int level = entry.mDPriceLevel();
    float price = entry.mDEntryPx().mantissa() *
                  std::pow(10, entry.mDEntryPx().exponent());
    int volume = entry.mDEntrySize();

    if (entry.mDEntryType() == MDEntryTypeBook::Bid) {
      HandleBidEntry(entry.mDUpdateAction(), level, price, volume);
    } else if (entry.mDEntryType() == MDEntryTypeBook::Offer) {
      HandleAskEntry(entry.mDUpdateAction(), level, price, volume);
    }

    ++seqnum_;
  }
}

void SymbolFeed::OnMDSnapshotFullRefreshOrderBook44(SnapshotFullRefreshOrderBook44 &) {

}

void SymbolFeed::OnMDIncrementalRefreshOrderBook43(MDIncrementalRefreshOrderBook43 &) {

}

void SymbolFeed::OnMDIncrementalRefreshDailyStatistics33(MDIncrementalRefreshDailyStatistics33 &) {

}

void SymbolFeed::OnMDIncrementalRefreshLimitsBanding34(MDIncrementalRefreshLimitsBanding34 &) {

}

void SymbolFeed::OnMDIncrementalRefreshSessionStatistics35(MDIncrementalRefreshSessionStatistics35 &) {

}

void SymbolFeed::OnMDIncrementalRefreshTrade36(MDIncrementalRefreshTrade36 &) {

}

void SymbolFeed::OnMDIncrementalRefreshVolume37(MDIncrementalRefreshVolume37 &) {

}

void SymbolFeed::OnMDIncrementalRefreshTradeSummary42(MDIncrementalRefreshTradeSummary42 &) {

}
