#ifndef CHANNEL_HH
#define CHANNEL_HH

#include <QObject>
#include <QAbstractTableModel>

#include "configobject.hh"
#include "signaling.hh"

class Config;
class RXGroupList;
class DigitalContact;
class ScanList;
class APRSSystem;
class PositioningSystem;
class RoamingZone;
class RadioID;


/** The base class of all channels (analog and digital) of a codeplug configuration.
 *
 * This class holds the common configuration of @c AnalogChannel and @c DigitalChannel, that is
 * the name, RX and TX frequencies, output power, TOT and default scanlist properties.
 *
 * @ingroup conf */
class Channel: public ConfigObject
{
	Q_OBJECT

public:
  /** Possible power settings. */
	typedef enum {
    MaxPower,  ///< Highest power setting (e.g. > 5W, if available).
    HighPower, ///< High power setting (e.g, 5W).
    MidPower,  ///< Medium power setting (e.g., 2W, if avaliable)
    LowPower,  ///< Low power setting (e.g., 1W).
    MinPower   ///< Lowest power setting (e.g., <1W, if available).
	} Power;

protected:
  /** Hidden constructor.
   * Constructs a new base channel.
   * @param name      Specifies the name of the channel.
   * @param rx        Sepcifies the RX freqeuncy in MHz.
   * @param tx        Specifies the TX frequency in MHz.
   * @param power     Specifies the power setting for the channel.
   * @param txTimeout Specifies the transmit timeout in seconds (TOT).
   * @param rxOnly    Specifies whether the channel is RX only.
   * @param scanlist  Specifies the default scanlist for the channel.
   * @param parent    Specified the @c QObject parent object. */
  Channel(const QString &name, double rx, double tx, Power power, uint txTimeout, bool rxOnly,
          ScanList *scanlist, QObject *parent=nullptr);

public:
  using ConfigObject::serialize;

  /** Returns @c true if the channel is of type @c T. This can be used to text wheter this channel
   * is actuially an analog or digital channel: @c channel->is<AnalogChannel>(). */
  template<class T>
  bool is() const {
	  return 0 != dynamic_cast<const T *>(this);
  }

  /** Dynamic cast of channels. For example @c channel->as<AnalogChannel>(). */
  template<class T>
  T *as() {
	  return dynamic_cast<T *>(this);
  }

  /** Dynamic cast of channels. For example @c channel->as<AnalogChannel>(). */
  template<class T>
  const T *as() const{
	  return dynamic_cast<const T *>(this);
  }

  /** Returns the name of the channel. */
  const QString &name() const;
  /** (Re-)Sets the name of the channel. */
  bool setName(const QString &name);

  /** Returns the RX frequency of the channel in MHz. */
  double rxFrequency() const;
  /** (Re-)Sets the RX frequency of the channel in MHz. */
  bool setRXFrequency(double freq);
  /** Returns the TX frequency of the channel in MHz. */
  double txFrequency() const;
  /** (Re-)Sets the TX frequency of the channel in MHz. */
  bool setTXFrequency(double freq);

  /** Returns the power setting of the channel. */
  Power power() const;
  /** (Re-)Sets the power setting of the channel. */
  void setPower(Power power);

  /** Returns the TX timeout (TOT) in seconds. */
  uint txTimeout() const;
  /** (Re-)Sets the TX timeout (TOT) in seconds. */
  bool setTimeout(uint dur);

  /** Returns @c true, if the channel is RX only. */
  bool rxOnly() const;
  /** Set, whether the channel is RX only. */
  bool setRXOnly(bool enable);

  /** Returns the default scan list for the channel. */
  ScanList *scanList() const;
  /** (Re-) Sets the default scan list for the channel. */
  bool setScanList(ScanList *list);

protected:
  bool serialize(YAML::Node &node, const Context &context);

protected slots:
  /** Internal hander for deleted scan lists. */
  void onScanListDeleted(QObject *obj);

protected:
  /** The channel name. */
  QString _name;
  /** The RX frequency in MHz. */
  double _rxFreq;
  /** The TX frequency in MHz. */
  double _txFreq;
  /** The transmit power setting. */
  Power _power;
  /** Transmit timeout in seconds. */
  uint  _txTimeOut;
  /** RX only flag. */
  bool  _rxOnly;
  /** Default scan list of the channel. */
  ScanList *_scanlist;
};


/** Extension to the @c Channel class to implement an analog channel.
 *
 * This class implements all the properties specific to an analog channel. That is, the admit
 * criterion, squelch, RX and TX tones and bandwidth settings.
 *
 * @ingroup conf */
class AnalogChannel: public Channel
{
  Q_OBJECT

public:
  /** Admit criteria of analog channel. */
	typedef enum {
		AdmitNone,  ///< Allow always.
    AdmitFree,  ///< Allow when channel free.
    AdmitTone   ///< Allow when admit tone is present.
	} Admit;

  /** Possible bandwidth of an analog channel. */
	typedef enum {
		BWNarrow,  ///< Narrow bandwidth (12.5kHz).
    BWWide     ///< Wide bandwidth (25kHz).
	} Bandwidth;

public:
  /** Constructs a new analog channel.
   *
   * @param name      Specifies the name of the channel.
   * @param rxFreq    Sepcifies the RX freqeuncy in MHz.
   * @param txFreq    Specifies the TX frequency in MHz.
   * @param power     Specifies the power setting for the channel.
   * @param txTimeout Specifies the transmit timeout in seconds (TOT).
   * @param rxOnly    Specifies whether the channel is RX only.
   * @param admit     Specifies the admit criterion.
   * @param squelch   Specifies the squelch level [0,10].
   * @param rxTone    Specifies CTCSS/DCS RX tone/code.
   * @param txTone    Specifies CTCSS/DCS TX tone/code.
   * @param bw        Specifies the bandwidth.
   * @param list      Specifies the default scanlist for the channel.
   * @param aprsSys   Specifies the APRS system for the channel.
   * @param parent    Specified the @c QObject parent object. */
  AnalogChannel(const QString &name, double rxFreq, double txFreq, Power power, uint txTimeout,
                bool rxOnly, Admit admit, uint squelch, Signaling::Code rxTone,
                Signaling::Code txTone, Bandwidth bw, ScanList *list,
                APRSSystem *aprsSys=nullptr, QObject *parent=nullptr);

  YAML::Node serialize(const Context &context);

  /** Returns the admit criterion for the analog channel. */
	Admit admit() const;
  /** (Re-)Sets the admit criterion for the analog channel. */
	void setAdmit(Admit admit);

  /** Returns the squelch level [0,10]. */
	uint squelch() const;
  /** (Re-)Sets the squelch level [0,10]. 0 Disables squelch (on some radios). */
	bool setSquelch(uint squelch);

  /** Returns the CTCSS/DCS RX tone, @c SIGNALING_NONE means disabled. */
  Signaling::Code rxTone() const;
  /** (Re-)Sets the CTCSS/DCS RX tone, @c SIGNALING_NONE disables the RX tone. */
  bool setRXTone(Signaling::Code code);
  /** Returns the CTCSS/DCS TX tone, @c SIGNALING_NONE means disabled. */
  Signaling::Code txTone() const;
  /** (Re-)Sets the CTCSS/DCS TX tone, @c SIGNALING_NONE disables the TX tone. */
  bool setTXTone(Signaling::Code code);

  /** Returns the bandwidth of the analog channel. */
	Bandwidth bandwidth() const;
  /** (Re-)Sets the bandwidth of the analog channel. */
	bool setBandwidth(Bandwidth bw);

  /** Returns the APRS system used for this channel or @c nullptr if disabled. */
  APRSSystem *aprsSystem() const;
  /** Sets the APRS system. */
  void setAPRSSystem(APRSSystem *sys);

protected:
  bool serialize(YAML::Node &node, const Context &context);

protected slots:
  /** Internal call-back that gets called if the associated APRS gets deleted. */
  void onAPRSSystemDeleted();

protected:
  /** Holds the admit criterion. */
	Admit _admit;
  /** Holds the squelch level [0,10]. */
	uint  _squelch;
  /** The RX CTCSS tone. */
  Signaling::Code _rxTone;
  /** The TX CTCSS tone. */
  Signaling::Code _txTone;
  /** The channel bandwidth. */
	Bandwidth _bw;
  /** A reference to the APRS system used on the channel or @c nullptr if disabled. */
  APRSSystem *_aprsSystem;
};



/** Extension to the @c Channel class to implement an digital (DMR) channel.
 *
 * That is, the admit criterion, color code, time slot, RX group list and TX contact.
 *
 * @ingroup conf */
class DigitalChannel: public Channel
{
	Q_OBJECT

public:
  /** Possible admit criteria of digital channels. */
	typedef enum {
		AdmitNone,      ///< No admit criteria, allows to transmit any time.
    AdmitFree,      ///< Transmit only if channel is free.
    AdmitColorCode  ///< Transmit only if channel is free and matches given color code.
	} Admit;

  /** Possible timeslots for digital channels. */
	typedef enum {
		TimeSlot1, ///< Time/repeater slot 1
    TimeSlot2  ///< Time/repeater slot 2
	} TimeSlot;

public:
  /** Constructs a new digital (DMR) channel.
   *
   * @param name      Specifies the name of the channel.
   * @param rxFreq    Sepcifies the RX freqeuncy in MHz.
   * @param txFreq    Specifies the TX frequency in MHz.
   * @param power     Specifies the power setting for the channel.
   * @param txTimeout Specifies the transmit timeout in seconds (TOT).
   * @param rxOnly    Specifies whether the channel is RX only.
   * @param admit     Specifies the admit criterion.
   * @param colorCode Specifies the colorcode [1,15].
   * @param timeslot  Specifies the time-slot.
   * @param rxGroup   Specifies the RX group list for the channel.
   * @param txContact Specifies the default TX contact to call on this channel.
   * @param posSystem Specifies the positioning system to use on this channel.
   * @param list      Specifies the default scanlist for the channel.
   * @param roaming   Specifies the roaming zone for the channel.
   * @param radioID   Specifies the radio ID to use for this channel, @c nullptr is default ID.
   * @param parent    Specified the @c QObject parent object. */
  DigitalChannel(const QString &name, double rxFreq, double txFreq, Power power, uint txTimeout,
	               bool rxOnly, Admit admit, uint colorCode, TimeSlot timeslot, RXGroupList *rxGroup,
                 DigitalContact *txContact, PositioningSystem *posSystem, ScanList *list,
                 RoamingZone *roaming, RadioID *radioID, QObject *parent=nullptr);

  YAML::Node serialize(const Context &context);

  /** Returns the admit criterion for the channel. */
	Admit admit() const;
  /** (Re-)Sets the admit criterion for the channel. */
	void setAdmit(Admit admit);

  /** Returns the color code for the channel. */
	uint colorCode() const;
  /** (Re-)Sets the color code for the channel. */
	bool setColorCode(uint cc);

  /** Returns the time slot for the channel. */
	TimeSlot timeslot() const;
  /** (Re-)Sets the time slot for the channel. */
	bool setTimeSlot(TimeSlot ts);

  /** Retruns the RX group list for the channel. */
	RXGroupList *rxGroupList() const;
  /** (Re-)Sets the RX group list for the channel. */
	bool setRXGroupList(RXGroupList *rxg);

  /** Returns the default TX contact to call on this channel. */
	DigitalContact *txContact() const;
  /** (Re-) Sets the default TX contact for this channel. */
	bool setTXContact(DigitalContact *c);

  /** Returns the GPS system associated with this channel or @c nullptr if not set. */
  PositioningSystem *posSystem() const;
  /** Associates the GPS System with this channel. */
  bool setPosSystem(PositioningSystem *sys);

  /** Returns the roaming zone associated with this channel or @c nullptr if not set. */
  RoamingZone *roaming() const;
  /** Associates the given roaming zone with this channel. */
  bool setRoaming(RoamingZone *zone);

  /** Returns the radio ID associated with this channel or @c nullptr if the default ID is used. */
  RadioID *radioId() const;
  /** Associates the given radio ID with this channel. Pass nullptr to set to default ID. */
  bool setRadioId(RadioID *id);

protected:
  bool serialize(YAML::Node &node, const Context &context);

protected slots:
  /** Internal callback if RX group list is deleted. */
	void onRxGroupDeleted();
  /** Internal callback if TX contact is deleted. */
	void onTxContactDeleted();
  /** Internal callback if GPS system is deleted. */
  void onPosSystemDeleted();
  /** Internal callback if roaming zone is deleted. */
  void onRoamingZoneDeleted();
  /** Internal used callback if radio ID is deleted. */
  void onRadioIdDeleted();

protected:
  /** The admit criterion. */
	Admit _admit;
  /** The channel color code. */
	uint _colorCode;
  /** The time slot for the channel. */
	TimeSlot _timeSlot;
  /** The RX group list for this channel. */
	RXGroupList *_rxGroup;
  /** The default TX contact. */
	DigitalContact *_txContact;
  /** The GPS system. */
  PositioningSystem *_posSystem;
  /** Roaming zone for the channel. */
  RoamingZone *_roaming;
  /** Radio ID to use on this channel. @c nullptr if default ID is used. */
  RadioID *_radioId;
};


/** Internal singleton class representing the "currently selected" channel.
 * @ingroup conf */
class SelectedChannel: public Channel
{
  Q_OBJECT

protected:
  /** Constructs the "selected" channel.
   * @warning Do not use this class directly, call @c SelectedChannel::get() instead. */
  explicit SelectedChannel();

public:
  /** Destructor. */
  virtual ~SelectedChannel();

  /** Constructs/gets the singleton instance. */
  static SelectedChannel *get();

protected:
  /** Holds the channel singleton instance. */
  static SelectedChannel *_instance;
};


/** Container class holding all channels (analog and digital) for a specific configuration
 * (@c Config).
 *
 * This class also implements the QAbstractTableModel and can therefore be displayed using a
 * default QTableView instance.
 *
 * @ingroup conf */
class ChannelList: public ConfigObjectList
{
	Q_OBJECT

public:
  /** Constructs an empty channel list. */
	explicit ChannelList(QObject *parent=nullptr);

  int add(ConfigObject *obj, int row=-1);

  /** Gets the channel at the specified index. */
  Channel *channel(int idx) const;
  /** Finds a digial channel with the given frequencies, time slot and color code. */
  DigitalChannel *findDigitalChannel(double rx, double tx, DigitalChannel::TimeSlot ts, uint cc) const;
  /** Finds an analog channel with the given frequeny. */
  AnalogChannel *findAnalogChannelByTxFreq(double freq) const;
};


#endif // CHANNEL_HH
