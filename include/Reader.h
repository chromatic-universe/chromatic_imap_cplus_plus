#ifndef __READER_H__
#define __READER_H__

#include <memory>

template <typename TStructure, typename TListener>
class Reader {
	typedef typename TypeTraits<TStructure>::Reader ReaderType;
	typename ReaderType::_var_type reader_i;
	std::tr1::shared_ptr<TListener> listener_;

public:
	Reader() {}
	Reader(DDS::DataReader_ptr reader, TListener *listener) : reader_i(ReaderType::_duplicate(ReaderType::_narrow(reader))), listener_(listener) {}

	TListener *GetListener() { return listener_.get(); }

	bool WaitForPublisherToComplete() {
		// Block until Publisher completes
		DDS::StatusCondition_var condition = reader_i->get_statuscondition();
		condition->set_enabled_statuses(DDS::SUBSCRIPTION_MATCHED_STATUS);

		DDS::WaitSet_var ws = new DDS::WaitSet;
		ws->attach_condition(condition);

		DDS::ConditionSeq conditions;
		DDS::SubscriptionMatchedStatus matches = { 0, 0, 0, 0, 0 };
		DDS::Duration_t timeout = { 30, 0 }; // 30 seconds

		do {
			if (ws->wait(conditions, timeout) != DDS::RETCODE_OK)
				return false;
			if (reader_i->get_subscription_matched_status(matches) != DDS::RETCODE_OK)
				return false;
		} while (matches.current_count > 0);

		ws->detach_condition(condition);
		return true;
	}
};

#endif
