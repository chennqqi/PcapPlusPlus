#ifndef PCAPPP_FILE_DEVICE
#define PCAPPP_FILE_DEVICE

#include <PcapDevice.h>
#include <RawPacket.h>

/// @file

/**
* \namespace pcpp
* \brief The main namespace for the PcapPlusPlus lib
*/
namespace pcpp
{

	/**
	 * @class IPcapFileDevice
	 * An abstract class (cannot be instantiated, has a private c'tor) that is the parent class of PcapFileReaderDevice and PcapFileWriterDevice
	 */
	class IPcapFileDevice : public IPcapDevice
	{
	protected:
		char* m_FileName;
		LinkLayerType m_PcapLinkLayerType;

		IPcapFileDevice(const char* fileName);
		virtual ~IPcapFileDevice();

	public:
		//override methods

		/**
		 * Close the pcap file
		 */
		virtual void close();
	};

	/**
	 * @class PcapFileReaderDevice
	 * A class for opening a pcap file in read-only mode. This class enable to open the file and read all packets, packet-by-packet
	 */
	class PcapFileReaderDevice : public IPcapFileDevice
	{
	private:
		uint32_t m_NumOfPacketsRead;
		uint32_t m_NumOfPacketsNotParsed;

		// private copy c'tor
		PcapFileReaderDevice(const PcapFileReaderDevice& other);
		PcapFileReaderDevice& operator=(const PcapFileReaderDevice& other);

	public:
		/**
		 * A constructor for this class that gets the pcap full path file name to open. Notice that after calling this constructor the file
		 * isn't opened yet, so reading packets will fail. For opening the file call open()
		 * @param[in] fileName The full path of the file to read
		 */
		PcapFileReaderDevice(const char* fileName);

		/**
		 * A destructor for this class
		 */
		~PcapFileReaderDevice();

		/**
		 * Read the next packet from the file. Before using this method please verify the file is opened using open()
		 * @param[out] rawPacket A reference for an empty RawPacket where the packet will be written
		 * @return True if a packet was read successfully. False will be returned if the file isn't opened (also, an error log will be printed)
		 * or if reached end-of-file
		 */
		bool getNextPacket(RawPacket& rawPacket);

		//override methods

		/**
		 * Open the file name which path was specified in the constructor in a read-only mode
		 * @return True if file was opened successfully or if file is already opened. False if opening the file failed for some reason (for example:
		 * file path does not exist)
		 */
		virtual bool open();

		/**
		 * Get statistics of packets read so far. In the pcap_stat struct, only ps_recv member is relevant. The rest of the members will contain 0
		 * @param[out] stats The stats struct where stats are returned
		 */
		virtual void getStatistics(pcap_stat& stats);
	};

	/**
	 * @class PcapFileWriterDevice
	 * A class for opening a pcap file for writing or create a new pcap file and write packets to it. This class adds
	 * a unique capability that isn't supported in WinPcap and in older libpcap versions which is to open a pcap file
	 * in append mode where packets are written at the end of the pcap file instead of running it over
	 */
	class PcapFileWriterDevice : public IPcapFileDevice
	{
	private:
		pcap_dumper_t* m_PcapDumpHandler;
		uint32_t m_NumOfPacketsWritten;
		uint32_t m_NumOfPacketsNotWritten;
		bool m_AppendMode;
		FILE* m_File;

		// private copy c'tor
		PcapFileWriterDevice(const PcapFileWriterDevice& other);
		PcapFileWriterDevice& operator=(const PcapFileWriterDevice& other);

		void closeFile();

	public:
		/**
		 * A constructor for this class that gets the pcap full path file name to open for writing or create. Notice that after calling this
		 * constructor the file isn't opened yet, so writing packets will fail. For opening the file call open()
		 * @param[in] fileName The full path of the file
		 * @param[in] linkLayerType The link layer type all packet in this file will be based on. The default is Ethernet
		 */
		PcapFileWriterDevice(const char* fileName, LinkLayerType linkLayerType = LINKTYPE_ETHERNET);

		/**
		 * A destructor for this class
		 */
		~PcapFileWriterDevice();

		/**
		 * Write a RawPacket to the file. Before using this method please verify the file is opened using open(). This method won't change the
		 * written packet
		 * @param[in] packet A reference for an existing RawPcket to write to the file
		 * @return True if a packet was written successfully. False will be returned if the file isn't opened
		 * or if the packet link layer type is different than the one defined for the file
		 * (in all cases, an error will be printed to log)
		 */
		bool writePacket(RawPacket const& packet);

		/**
		 * Write multiple RawPacket to the file. Before using this method please verify the file is opened using open(). This method won't change
		 * the written packets or the RawPacketVector instance
		 * @param[in] packets A reference for an existing RawPcketVector, all of its packets will be written to the file
		 * @return True if all packets were written successfully to the file. False will be returned if the file isn't opened (also, an error
		 * log will be printed) or if at least one of the packets wasn't written successfully to the file
		 */
		bool writePackets(const RawPacketVector& packets);

		//override methods

		/**
		 * Open the file in a write mode. If file doesn't exist, it will be created. If it does exist it will be
		 * overwritten, meaning all its current content will be deleted
		 * @return True if file was opened/created successfully or if file is already opened. False if opening the file failed for some reason
		 * (an error will be printed to log)
		 */
		virtual bool open();

		/**
		 * Same as open(), but enables to open the file in append mode in which packets will be appended to the file
		 * instead of overwrite its current content. In append mode file must exist, otherwise opening will fail
		 * @param[in] appendMode A boolean indicating whether to open the file in append mode or not. If set to false
		 * this method will act exactly like open(). If set to true, file will be opened in append mode
		 * @return True of managed to open the file successfully. In case appendMode is set to true, false will be returned
		 * if file wasn't found or couldn't be read, if file type is not pcap, or if link type specified in c'tor is
		 * different from current file link type. In case appendMode is set to false, please refer to open() for return
		 * values
		 */
		bool open(bool appendMode);

		/**
		 * Flush and close the pacp file
		 */
		virtual void close();

		/**
		 * Get statistics of packets written so far. In the pcap_stat struct, only ps_recv member is relevant. The rest of the members will contain 0
		 * @param[out] stats The stats struct where stats are returned
		 */
		virtual void getStatistics(pcap_stat& stats);
	};

}// namespace pcpp

#endif
