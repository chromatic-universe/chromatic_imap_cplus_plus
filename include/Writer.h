#ifndef __WRITER_H__
#define __WRITER_H__


template <typename TStructure>
class Writer {
	typedef typename TypeTraits<TStructure>::Writer WriterType;  // to avoid C4346
	typename WriterType::_var_type writer_i;
public:
	Writer() : writer_i(0) {}

	Writer(DDS::DataWriter_ptr writer) : writer_i(WriterType::_duplicate(WriterType::_narrow(writer))) {}

	bool WaitForSubscriber() {
		// Block until Subscriber is available
		DDS::StatusCondition_var condition = writer_i->get_statuscondition();
		condition->set_enabled_statuses(DDS::PUBLICATION_MATCHED_STATUS);

		DDS::WaitSet_var ws = new DDS::WaitSet;
		ws->attach_condition(condition);

		DDS::ConditionSeq conditions;
		DDS::PublicationMatchedStatus matches = { 0, 0, 0, 0, 0 };
		DDS::Duration_t timeout = { 30, 0 };  // 30 seconds

		do {
			if (ws->wait(conditions, timeout) != DDS::RETCODE_OK)
				return false;
			if (writer_i->get_publication_matched_status(matches) != ::DDS::RETCODE_OK)
				return false;
		} while (matches.current_count < 1);

		ws->detach_condition(condition);
		return true;
	}


	bool WaitForAcknowledgements() {
		// Wait for samples to be acknowledged
		DDS::Duration_t timeout = { 30, 0 };  // 30 seconds
		return (writer_i->wait_for_acknowledgments(timeout) == DDS::RETCODE_OK);
	}

	DDS::ReturnCode_t Write(const TStructure &s) {
		return writer_i->write(s, ::DDS::HANDLE_NIL);
	}
};


#endif