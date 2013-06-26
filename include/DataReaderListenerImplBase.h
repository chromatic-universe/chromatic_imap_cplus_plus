#ifndef __DATAREADERLISTENERIMPLBASE_H__
#define __DATAREADERLISTENERIMPLBASE_H__

#include <ace/Global_Macros.h>
#include <dds/DdsDcpsSubscriptionS.h>
#include <dds/DCPS/LocalObject.h>

template <typename TDataReader, typename TStructure>
class DataReaderListenerImplBase
	: public virtual OpenDDS::DCPS::LocalObject<DDS::DataReaderListener> {
public:
	virtual void on_requested_deadline_missed(
		DDS::DataReader_ptr,
		const DDS::RequestedDeadlineMissedStatus&) {}

	virtual void on_requested_incompatible_qos(
		DDS::DataReader_ptr,
		const DDS::RequestedIncompatibleQosStatus&) {}

	virtual void on_sample_rejected(
		DDS::DataReader_ptr,
		const DDS::SampleRejectedStatus&) {}

	virtual void on_liveliness_changed(
		DDS::DataReader_ptr,
		const DDS::LivelinessChangedStatus&) {}

	virtual void on_subscription_matched(
		DDS::DataReader_ptr,
		const DDS::SubscriptionMatchedStatus&) {}

	virtual void on_sample_lost(
		DDS::DataReader_ptr,
		const DDS::SampleLostStatus&) {}

	virtual void Process(const TStructure &) = 0;
	virtual void on_data_available(DDS::DataReader_ptr reader) {

		typename TDataReader::_var_type reader_i = TDataReader::_narrow(reader);

		// if an exception is thrown here, it is consumed by the transport thread, 
		// so ignore narrowing errors at this time
		if (0 == reader_i)
			return;

		TStructure sample;
		DDS::SampleInfo info;

		DDS::ReturnCode_t status = reader_i->take_next_sample(sample, info);

		while (status == DDS::RETCODE_OK) {
			if (info.valid_data)
				Process(sample);
			status = reader_i->take_next_sample(sample, info);
		} 
	}
};

#endif
