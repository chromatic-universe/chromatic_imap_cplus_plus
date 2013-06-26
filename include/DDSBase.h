#ifndef __DDSBASE_H__
#define __DDSBASE_H__

class DDSException : public std::exception {
public:
	DDSException(const char *const& _What) : std::exception(_What) {}
};

class DDSBase {
	typedef std::map<::DDS::DomainId_t, DDS::DomainParticipant_var> DomainParticipantMap;
	DDS::DomainParticipantFactory_var dpf_;
	DomainParticipantMap participantMap_;
protected:
	DDS::DomainParticipant_ptr GetParticipant(::DDS::DomainId_t domainId) {
    	DomainParticipantMap::iterator it=participantMap_.find(domainId);
	    if (it==participantMap_.end()) {

    		::DDS::DomainParticipant_ptr participant = dpf_->create_participant(domainId,
    			PARTICIPANT_QOS_DEFAULT,
    			0,
    			OpenDDS::DCPS::DEFAULT_STATUS_MASK);
    
    		if (0 == participant)
				throw DDSException("DDSBase::GetParticipant(): create_participant failed");
    
    		participantMap_[domainId] = participant;
    	}
    
		return ::DDS::DomainParticipant::_duplicate(participantMap_[domainId].in());
	}
	
	
public:
	DDSBase(int argc, ACE_TCHAR *argv[]) {
	    dpf_ = TheParticipantFactoryWithArgs(argc, argv);
	}
	
	~DDSBase() {
    	for (DomainParticipantMap::iterator it=participantMap_.begin(); it!=participantMap_.end(); it++) {
   			it->second->delete_contained_entities();
    		dpf_->delete_participant(it->second.in());
    	}
    
    	TheTransportFactory->release();
    	TheServiceParticipant->shutdown();
	}


	template <typename TStructure>
	Writer<TStructure> CreateWriter(::DDS::DomainId_t domainId, const char *topic_name) {

		DDS::DomainParticipant_var participant = GetParticipant(domainId);

		// Create Topic
		typename TypeTraits<TStructure>::TypeSupport::_var_type ts = new TypeTraits<TStructure>::TypeSupportImpl();

		if (ts->register_type(participant, "") != DDS::RETCODE_OK) 
			throw DDSException("DDSBase::CreateWriter(): register_type failed");

		CORBA::String_var type_name = ts->get_type_name();
		DDS::Topic_var topic =
			participant->create_topic(topic_name,
			type_name.in(),
			TOPIC_QOS_DEFAULT,
			0,
			OpenDDS::DCPS::DEFAULT_STATUS_MASK);

		if (0 == topic)
			throw DDSException("DDSBase::CreateWriter(): create_topic failed");

		// Create Publisher
		DDS::Publisher_var publisher =
			participant->create_publisher(PUBLISHER_QOS_DEFAULT,
			0,
			OpenDDS::DCPS::DEFAULT_STATUS_MASK);

		if (0 == publisher)
			throw DDSException("DDSBase::CreateWriter(): create_publisher failed");

		// Initialize and attach Transport
		OpenDDS::DCPS::TransportImpl_rch transport_impl =
			TheTransportFactory->create_transport_impl(OpenDDS::DCPS::DEFAULT_SIMPLE_TCP_ID,
			OpenDDS::DCPS::AUTO_CONFIG);

		if (transport_impl->attach(publisher.in()) != OpenDDS::DCPS::ATTACH_OK)
			throw DDSException("DDSBase::CreateWriter(): transport_impl->attach failed");

		// Create DataWriter
		DDS::DataWriter_var writer =
			publisher->create_datawriter(topic.in(),
			DATAWRITER_QOS_DEFAULT,
			0,
			OpenDDS::DCPS::DEFAULT_STATUS_MASK);

		if (0 == writer)
			throw DDSException("DDSBase::CreateWriter(): create_datawriter failed");

		return Writer<TStructure>(writer.in());
	}

	
	template <typename TStructure, typename TListener>
	Reader<TStructure, TListener> CreateReader(::DDS::DomainId_t domainId, const char *topic_name, TListener *listener) {

		DDS::DomainParticipant_var participant = GetParticipant(domainId);

		// Create Topic
		typename TypeTraits<TStructure>::TypeSupport::_var_type ts = new TypeTraits<TStructure>::TypeSupportImpl();

		if (ts->register_type(participant, "") != DDS::RETCODE_OK)
			throw DDSException("DDSBase::CreateReader(): register_type failed");

		CORBA::String_var type_name = ts->get_type_name();
		DDS::Topic_var topic =
			participant->create_topic(topic_name,
			type_name.in(),
			TOPIC_QOS_DEFAULT,
			0,
			OpenDDS::DCPS::DEFAULT_STATUS_MASK);

		if (0 == topic)
			throw DDSException("DDSBase::CreateReader(): create_topic failed");

		// Create Subscriber
		DDS::Subscriber_var subscriber =
			participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
			0,
			OpenDDS::DCPS::DEFAULT_STATUS_MASK);

		if (0 == subscriber)
			throw DDSException("DDSBase::CreateReader(): create_subscriber failed");

		// Initialize and attach Transport
		OpenDDS::DCPS::TransportImpl_rch transport_impl =
			TheTransportFactory->create_transport_impl(OpenDDS::DCPS::DEFAULT_SIMPLE_TCP_ID,
			OpenDDS::DCPS::AUTO_CONFIG);

		OpenDDS::DCPS::AttachStatus status = transport_impl->attach(subscriber.in());

		if (status != OpenDDS::DCPS::ATTACH_OK)
			throw DDSException("DDSBase::CreateReader(): transport_impl->attach failed");

		// Create DataReader - set RELIABLE reliability
		DDS::DataReaderQos dr_qos;
		if (subscriber->get_default_datareader_qos(dr_qos) != ::DDS::RETCODE_OK)
			throw DDSException("DDSBase::CreateReader(): get_default_datareader_qos failed");

		dr_qos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;

		DDS::DataReader_var reader =
			subscriber->create_datareader(topic.in(),
			dr_qos,
			listener,
			OpenDDS::DCPS::DEFAULT_STATUS_MASK);

		if (0 == reader)
			throw DDSException("DDSBase::CreateReader(): create_datareader failed");

		return Reader<TStructure, TListener>(reader, listener);
	}


};

#endif
