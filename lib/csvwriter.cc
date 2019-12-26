#include "csvwriter.hh"
#include <QDateTime>
#include "config.hh"
#include "config.h"
#include <cmath>


inline QString formatFrequency(float f) {
  int val = std::round(f*10000);
  return QString::number(double(val)/10000, 'f', 4);
}

bool
CSVWriter::write(const Config *config, QTextStream &stream, QString &errorMessage)
{
  stream << "#\n"
         << "# Configuration generated " << QDateTime::currentDateTime().toString()
         << " by qdrm, version " VERSION_STRING "\n"
         << "# see https://dm3mat.darc.de/qdmr for details.\n"
         << "#\n\n";


  stream << "# Unique DMR ID and name (quoted) of this radio.\n"
         << "ID: " << config->id() << "\n"
         << "Name: \"" << config->name() << "\"\n\n"
         << "# Text displayed when the radio powers up (quoted).\n"
         << "IntroLine1: \"" << config->introLine1() << "\"\n"
         << "IntroLine2: \"" << config->introLine2() << "\"\n\n"
         << "# Microphone amplification, value 1..10:\n"
         << "MICLevel: " << config->micLevel() << "\n\n"
         << "# Speech-synthesis ('On' or 'Off'):\n"
         << "Speech: " << (config->speech() ? "On" : "Off") << "\n\n";

  stream << "# Table of digital channels.\n"
            "# 1) Channel number: 1-1024\n"
            "# 2) Name in quotes. E.g., \"NAME\" \n"
            "# 3) Receive frequency in MHz\n"
            "# 4) Transmit frequency or +/- offset in MHz\n"
            "# 5) Transmit power: High, Low\n"
            "# 6) Scan list: - or index in Scanlist table\n"
            "# 7) Transmit timeout timer in seconds: 0, 15, 30, 45... 555\n"
            "# 8) Receive only: -, +\n"
            "# 9) Admit criteria: -, Free, Color\n"
            "# 10) Color code: 0, 1, 2, 3... 15\n"
            "# 11) Time slot: 1 or 2\n"
            "# 12) Receive group list: - or index in Grouplist table\n"
            "# 13) Contact for transmit: - or index in Contacts table\n"
            "# 14) GPS System: - or index in GPS table.\n"
            "#\n"
            "Digital Name                Receive   Transmit  Power Scan TOT RO Admit  CC TS RxGL TxC GPS\n";
  for (int i=0; i<config->channelList()->count(); i++) {
    if (config->channelList()->channel(i)->is<AnalogChannel>())
      continue;
    DigitalChannel *digi = config->channelList()->channel(i)->as<DigitalChannel>();
    stream << qSetFieldWidth(8)  << left << (i+1)
           << qSetFieldWidth(20) << left << ("\"" + digi->name() + "\"")
           << qSetFieldWidth(10) << left << formatFrequency(digi->rxFrequency());
    if (digi->txFrequency()<digi->rxFrequency())
      stream << qSetFieldWidth(10) << left << formatFrequency(digi->txFrequency()-digi->rxFrequency());
    else
      stream << qSetFieldWidth(10) << left << formatFrequency(digi->txFrequency());
    stream << qSetFieldWidth(6)  << left << ( (Channel::HighPower == digi->power()) ? "High" : "Low" )
           << qSetFieldWidth(5)  << left << ( nullptr != digi->scanList() ?
          QString::number(config->scanlists()->indexOf(digi->scanList())+1) : QString("-") )
           << qSetFieldWidth(4)  << left << ( (0 == digi->txTimeout()) ? '-' : digi->txTimeout() )
           << qSetFieldWidth(3)  << left << (digi->rxOnly() ? '+' : '-')
           << qSetFieldWidth(7)  << left << ((DigitalChannel::AdmitNone==digi->admit()) ? "-" : ((DigitalChannel::AdmitFree==digi->admit()) ? "Free" : "Color"))
           << qSetFieldWidth(3)  << left << digi->colorCode()
           << qSetFieldWidth(3)  << left << (DigitalChannel::TimeSlot1==digi->timeslot() ? "1" : "2");
    if (nullptr == digi->rxGroupList())
      stream << qSetFieldWidth(5)  << left << '-';
    else
      stream << qSetFieldWidth(5) << left << (config->rxGroupLists()->indexOf(digi->rxGroupList())+1);
    if (nullptr == digi->txContact())
      stream << qSetFieldWidth(4)  << left << '-';
    else
      stream << qSetFieldWidth(4) << left << (config->contacts()->indexOf(digi->txContact())+1);
    if (nullptr == digi->gpsSystem())
      stream << qSetFieldWidth(4) << left << "-";
    else
      stream << qSetFieldWidth(4) << left << config->gpsSystems()->indexOf(digi->gpsSystem())+1;
    if (digi->txContact())
      stream << qSetFieldWidth(0) << "# " << digi->txContact()->name();
    stream << qSetFieldWidth(0) << "\n";
  }
  stream << "\n";

  stream << "# Table of analog channels.\n"
            "# 1) Channel number: 1-1024\n"
            "# 2) Name in quotes.\n"
            "# 3) Receive frequency in MHz\n"
            "# 4) Transmit frequency or +/- offset in MHz\n"
            "# 5) Transmit power: High, Low\n"
            "# 6) Scan list: - or index\n"
            "# 7) Transmit timeout timer in seconds: 0, 15, 30, 45... 555\n"
            "# 8) Receive only: -, +\n"
            "# 9) Admit criteria: -, Free, Tone\n"
            "# 10) Squelch level: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9\n"
            "# 11) Guard tone for receive, or '-' to disable\n"
            "# 12) Guard tone for transmit, or '-' to disable\n"
            "# 13) Bandwidth in kHz: 12.5, 25\n"
            "#\n"
            "Analog  Name                Receive    Transmit Power Scan TOT RO Admit  Squelch RxTone TxTone Width\n";
  for (int i=0; i<config->channelList()->count(); i++) {
    if (config->channelList()->channel(i)->is<DigitalChannel>())
      continue;
    AnalogChannel *analog = config->channelList()->channel(i)->as<AnalogChannel>();
    stream << qSetFieldWidth(8)  << left << (i+1)
           << qSetFieldWidth(20) << left << ("\"" + analog->name() + "\"")
           << qSetFieldWidth(10) << left << formatFrequency(analog->rxFrequency());
    if (analog->txFrequency()<analog->rxFrequency())
      stream << qSetFieldWidth(10) << left << formatFrequency(analog->txFrequency()-analog->rxFrequency());
    else
      stream << qSetFieldWidth(10) << left << formatFrequency(analog->txFrequency());
    stream << qSetFieldWidth(6)  << left << ( (Channel::HighPower == analog->power()) ? "High" : "Low" )
           << qSetFieldWidth(5)  << left << ( nullptr != analog->scanList() ?
          QString::number(config->scanlists()->indexOf(analog->scanList())+1) : QString("-") )
           << qSetFieldWidth(4)  << left << ( (0 == analog->txTimeout()) ? '-' : analog->txTimeout() )
           << qSetFieldWidth(3)  << left << (analog->rxOnly() ? '+' : '-')
           << qSetFieldWidth(7)  << left << ((AnalogChannel::AdmitNone==analog->admit()) ? "-" : ((AnalogChannel::AdmitFree==analog->admit()) ? "Free" : "Tone"))
           << qSetFieldWidth(8)  << left << analog->squelch();
    if (0 == analog->rxTone())
      stream << qSetFieldWidth(7)  << left << "-";
    else
      stream << qSetFieldWidth(7)  << left << analog->rxTone();
    if (0 == analog->txTone())
      stream << qSetFieldWidth(7)  << left << "-";
    else
      stream << qSetFieldWidth(7)  << left << analog->txTone();
    stream << qSetFieldWidth(5) << left << (AnalogChannel::BWWide == analog->bandwidth() ? 25.0 : 12.5);
    stream << qSetFieldWidth(0) << "\n";
  }
  stream << "\n";

  stream << "# Table of channel zones.\n"
            "# 1) Zone number\n"
            "# 2) Name in quotes. \n"
            "# 3) VFO: Either A or B.\n"
            "# 4) List of channels: numbers and ranges (N-M) separated by comma\n"
            "#\n"
            "Zone    Name                VFO Channels\n";
  for (int i=0; i<config->zones()->count(); i++) {
    Zone *zone = config->zones()->zone(i);
    if (zone->A()->count()) {
      stream << qSetFieldWidth(8)  << left << (i+1)
             << qSetFieldWidth(20) << left << ("\"" + zone->name() + "\"")
             << qSetFieldWidth(4)  << left << "A";
      QStringList tmp;
      for (int j=0; j<zone->A()->count(); j++) {
        tmp.append(QString::number(config->channelList()->indexOf(zone->A()->channel(j))+1));
      }
      stream << qSetFieldWidth(0) << tmp.join(",") << "\n";
    }
    if (zone->B()->count()) {
      stream << qSetFieldWidth(8)  << left << (i+1)
             << qSetFieldWidth(20) << left << ("\"" + zone->name() + "\"")
             << qSetFieldWidth(4)  << left << "B";
      QStringList tmp;
      for (int j=0; j<zone->B()->count(); j++) {
        tmp.append(QString::number(config->channelList()->indexOf(zone->B()->channel(j))+1));
      }
      stream << qSetFieldWidth(0) << tmp.join(",") << "\n";
    }
  }
  stream << "\n";

  stream << "# Table of scan lists.\n"
            "# 1) Scan list number: 1-250\n"
            "# 2) Name in quotes.\n"
            "# 3) Priority channel 1 (50% of scans): -, Sel or index\n"
            "# 4) Priority channel 2 (25% of scans): -, Sel or index\n"
            "# 5) Designated transmit channel: Last, Sel or index\n"
            "# 6) List of channels: numbers and ranges (N-M) separated by comma\n"
            "#\n"
            "Scanlist Name               PCh1 PCh2 TxCh Channels\n";
  for (int i=0; i<config->scanlists()->count(); i++) {
    ScanList *list = config->scanlists()->scanlist(i);
    stream << qSetFieldWidth(9)  << left << (i+1)
           << qSetFieldWidth(20) << left << ("\"" + list->name() + "\"")
           << qSetFieldWidth(5)  << left
           << ( (0 == list->priorityChannel()) ?
                  "-" : QString::number(config->channelList()->indexOf(list->priorityChannel())+1) )
           << qSetFieldWidth(5)  << left
           << ( (0 == list->secPriorityChannel()) ?
                  "-" : QString::number(config->channelList()->indexOf(list->secPriorityChannel())+1) )
           << qSetFieldWidth(5)  << left << "Sel";
    QStringList tmp;
    for (int j=0; j<list->count(); j++) {
      tmp.append(QString::number(config->channelList()->indexOf(list->channel(j))+1));
    }
    stream << qSetFieldWidth(0) << tmp.join(",") << "\n";
  }
  stream << "\n";

  stream << "# Table of GPS systems.\n"
            "# 1) GPS system ID\n"
            "# 2) Name in quotes.\n"
            "# 3) Destination contact ID.\n"
            "# 4) Update period: period in ms\n"
            "# 5) Revert channel ID or '-'.\n"
            "#\n"
            "GPS  Name                Dest Period Revert\n";
  for (int i=0; i<config->gpsSystems()->count(); i++) {
    GPSSystem *gps = config->gpsSystems()->gpsSystem(i);
    stream << qSetFieldWidth(5)  << left << (i+1)
           << qSetFieldWidth(20) << left << ("\"" + gps->name() + "\"")
           << qSetFieldWidth(5)  << left << config->contacts()->indexOfDigital(gps->contact())+1
           << qSetFieldWidth(7)  << left << gps->period()
           << qSetFieldWidth(6)  << left
           << ( (gps->hasRevertChannel()) ? QString::number(config->channelList()->indexOf(gps->revertChannel())+1) : "-" )
           << "\n";
  }
  stream << "\n";

  stream << "# Table of contacts.\n"
            "# 1) Contact number: 1-256\n"
            "# 2) Name in quotes.\n"
            "# 3) Call type: Group, Private, All or DTMF\n"
            "# 4) Call ID: 1...16777215 or string with DTMF number\n"
            "# 5) Call receive tone: -, +\n"
            "#\n"
            "Contact Name                Type    ID          RxTone\n";
  for (int i=0; i<config->contacts()->count(); i++) {
    if (config->contacts()->contact(i)->is<DigitalContact>()) {
      DigitalContact *contact = config->contacts()->contact(i)->as<DigitalContact>();
      stream << qSetFieldWidth(8)  << left << (i+1)
             << qSetFieldWidth(20) << left << ("\"" + contact->name() + "\"")
             << qSetFieldWidth(8)  << left
             << (DigitalContact::PrivateCall == contact->type() ?
                   "Private" : (DigitalContact::GroupCall == contact->type() ?
                                  "Group" : "All"))
             << qSetFieldWidth(12)  << left << contact->number()
             << qSetFieldWidth(6)  << left << (contact->rxTone() ? "+" : "-");
      stream << qSetFieldWidth(0) << "\n";
    } else if (config->contacts()->contact(i)->is<DTMFContact>()) {
      DTMFContact *contact = config->contacts()->contact(i)->as<DTMFContact>();
      stream << qSetFieldWidth(8)  << left << (i+1)
             << qSetFieldWidth(17) << left << ("\"" + contact->name() + "\"")
             << qSetFieldWidth(8)  << left << "DTMF"
             << qSetFieldWidth(12) << left << ("\""+contact->number()+"\"")
             << qSetFieldWidth(6)  << left << (contact->rxTone() ? "+" : "-");
      stream << qSetFieldWidth(0) << "\n";
    }
  }
  stream << "\n";

  stream << "# Table of group lists.\n"
            "# 1) Group list number: 1-64\n"
            "# 2) Name in quotes.\n"
            "# 3) List of contacts: numbers and ranges (N-M) separated by comma\n"
            "#\n"
            "Grouplist Name                Contacts\n";
  for (int i=0; i<config->rxGroupLists()->count(); i++) {
    RXGroupList *list = config->rxGroupLists()->list(i);
    stream << qSetFieldWidth(10) << left << (i+1)
           << qSetFieldWidth(20) << left << ("\"" + list->name() + "\"");
    QStringList tmp;
    for (int j=0; j<list->count(); j++) {
      tmp.append(QString::number(config->contacts()->indexOf(list->contact(j))+1));
    }
    stream << qSetFieldWidth(0) << tmp.join(",") << "\n";
  }
  stream << "\n";

  return true;
}

