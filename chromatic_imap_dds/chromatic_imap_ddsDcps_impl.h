#ifndef CHROMATIC_IMAP_DDSDCPS_IMPL_H_
#define CHROMATIC_IMAP_DDSDCPS_IMPL_H_

#include "ccpp.h"
#include "ccpp_chromatic_imap_dds.h"
#include "ccpp_TypeSupport_impl.h"
#include "ccpp_DataWriter_impl.h"
#include "ccpp_DataReader_impl.h"
#include "ccpp_DataReaderView_impl.h"


namespace chromatic_imap_dds {

    class  chromatic_imap_commandTypeSupportFactory : public ::DDS::TypeSupportFactory_impl
    {
    public:
        chromatic_imap_commandTypeSupportFactory() {}
        virtual ~chromatic_imap_commandTypeSupportFactory() {}
    private:
        ::DDS::DataWriter_ptr
        create_datawriter (gapi_dataWriter handle);
    
        ::DDS::DataReader_ptr
        create_datareader (gapi_dataReader handle);
    
        ::DDS::DataReaderView_ptr
        create_view (gapi_dataReaderView handle);
    };
    
    class  chromatic_imap_commandTypeSupport : public virtual chromatic_imap_commandTypeSupportInterface,
                                   public ::DDS::TypeSupport_impl
    {
    public:
        virtual ::DDS::ReturnCode_t register_type(
            ::DDS::DomainParticipant_ptr participant,
            const char * type_name) THROW_ORB_EXCEPTIONS;
    
        virtual char * get_type_name() THROW_ORB_EXCEPTIONS;
    
        chromatic_imap_commandTypeSupport (void);
        virtual ~chromatic_imap_commandTypeSupport (void);
    
    private:
        chromatic_imap_commandTypeSupport (const chromatic_imap_commandTypeSupport &);
        void operator= (const chromatic_imap_commandTypeSupport &);
    
        static const char *metaDescriptor[];
        static const CORBA::ULong metaDescriptorArrLength;
    };
    
    typedef chromatic_imap_commandTypeSupportInterface_var chromatic_imap_commandTypeSupport_var;
    typedef chromatic_imap_commandTypeSupportInterface_ptr chromatic_imap_commandTypeSupport_ptr;
    
    class  chromatic_imap_commandDataWriter_impl : public virtual chromatic_imap_commandDataWriter,
                                        public ::DDS::DataWriter_impl
    {
    public:
    
        virtual ::DDS::InstanceHandle_t register_instance(
            const chromatic_imap_command & instance_data) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::InstanceHandle_t register_instance_w_timestamp(
            const chromatic_imap_command & instance_data,
            const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t unregister_instance(
            const chromatic_imap_command & instance_data,
            ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t unregister_instance_w_timestamp(
            const chromatic_imap_command & instance_data,
            ::DDS::InstanceHandle_t handle,
            const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t write(
            const chromatic_imap_command & instance_data,
            ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t write_w_timestamp(
            const chromatic_imap_command & instance_data,
            ::DDS::InstanceHandle_t handle,
            const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t dispose(
            const chromatic_imap_command & instance_data,
            ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t dispose_w_timestamp(
            const chromatic_imap_command & instance_data,
            ::DDS::InstanceHandle_t handle,
            const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t writedispose(
            const chromatic_imap_command & instance_data,
            ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t writedispose_w_timestamp(
            const chromatic_imap_command & instance_data,
            ::DDS::InstanceHandle_t handle,
            const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t get_key_value(
            chromatic_imap_command & key_holder,
            ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::InstanceHandle_t lookup_instance(
            const chromatic_imap_command & instance_data) THROW_ORB_EXCEPTIONS;
    
    
        chromatic_imap_commandDataWriter_impl (
            gapi_dataWriter handle
        );
    
        virtual ~chromatic_imap_commandDataWriter_impl (void);
    
    private:
        chromatic_imap_commandDataWriter_impl(const chromatic_imap_commandDataWriter_impl &);
        void operator= (const chromatic_imap_commandDataWriter &);
    };
    
    class  chromatic_imap_commandDataReader_impl : public virtual chromatic_imap_commandDataReader,
                                        public ::DDS::DataReader_impl
    {
        friend class chromatic_imap_commandDataReaderView_impl;
    public:
        virtual ::DDS::ReturnCode_t read(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_w_condition(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_w_condition(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_next_sample(
            chromatic_imap_command & received_data,
            ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_next_sample(
            chromatic_imap_command & received_data,
            ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_instance(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_instance(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_next_instance(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_next_instance(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t return_loan(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t get_key_value(
            chromatic_imap_command & key_holder,
            ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::InstanceHandle_t lookup_instance(
            const chromatic_imap_command & instance) THROW_ORB_EXCEPTIONS;
    
        chromatic_imap_commandDataReader_impl (
            gapi_dataReader handle
        );
    
        virtual ~chromatic_imap_commandDataReader_impl(void);
    
    private:
        chromatic_imap_commandDataReader_impl(const chromatic_imap_commandDataReader &);
        void operator= (const chromatic_imap_commandDataReader &);
    
        static ::DDS::ReturnCode_t check_preconditions(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples
        );
    };
    
    class  chromatic_imap_commandDataReaderView_impl : public virtual chromatic_imap_commandDataReaderView,
                                        public ::DDS::DataReaderView_impl
    {
    public:
        virtual ::DDS::ReturnCode_t read(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_w_condition(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_w_condition(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_next_sample(
            chromatic_imap_command & received_data,
            ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_next_sample(
            chromatic_imap_command & received_data,
            ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_instance(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_instance(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_next_instance(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_next_instance(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t return_loan(
            chromatic_imap_commandSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t get_key_value(
            chromatic_imap_command & key_holder,
            ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::InstanceHandle_t lookup_instance(
            const chromatic_imap_command & instance) THROW_ORB_EXCEPTIONS;
    
        chromatic_imap_commandDataReaderView_impl (
            gapi_dataReader handle
        );
    
        virtual ~chromatic_imap_commandDataReaderView_impl(void);
    
    private:
        chromatic_imap_commandDataReaderView_impl(const chromatic_imap_commandDataReaderView &);
        void operator= (const chromatic_imap_commandDataReaderView &);
    };
    
    class  chromatic_name_serviceTypeSupportFactory : public ::DDS::TypeSupportFactory_impl
    {
    public:
        chromatic_name_serviceTypeSupportFactory() {}
        virtual ~chromatic_name_serviceTypeSupportFactory() {}
    private:
        ::DDS::DataWriter_ptr
        create_datawriter (gapi_dataWriter handle);
    
        ::DDS::DataReader_ptr
        create_datareader (gapi_dataReader handle);
    
        ::DDS::DataReaderView_ptr
        create_view (gapi_dataReaderView handle);
    };
    
    class  chromatic_name_serviceTypeSupport : public virtual chromatic_name_serviceTypeSupportInterface,
                                   public ::DDS::TypeSupport_impl
    {
    public:
        virtual ::DDS::ReturnCode_t register_type(
            ::DDS::DomainParticipant_ptr participant,
            const char * type_name) THROW_ORB_EXCEPTIONS;
    
        virtual char * get_type_name() THROW_ORB_EXCEPTIONS;
    
        chromatic_name_serviceTypeSupport (void);
        virtual ~chromatic_name_serviceTypeSupport (void);
    
    private:
        chromatic_name_serviceTypeSupport (const chromatic_name_serviceTypeSupport &);
        void operator= (const chromatic_name_serviceTypeSupport &);
    
        static const char *metaDescriptor[];
        static const CORBA::ULong metaDescriptorArrLength;
    };
    
    typedef chromatic_name_serviceTypeSupportInterface_var chromatic_name_serviceTypeSupport_var;
    typedef chromatic_name_serviceTypeSupportInterface_ptr chromatic_name_serviceTypeSupport_ptr;
    
    class  chromatic_name_serviceDataWriter_impl : public virtual chromatic_name_serviceDataWriter,
                                        public ::DDS::DataWriter_impl
    {
    public:
    
        virtual ::DDS::InstanceHandle_t register_instance(
            const chromatic_name_service & instance_data) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::InstanceHandle_t register_instance_w_timestamp(
            const chromatic_name_service & instance_data,
            const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t unregister_instance(
            const chromatic_name_service & instance_data,
            ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t unregister_instance_w_timestamp(
            const chromatic_name_service & instance_data,
            ::DDS::InstanceHandle_t handle,
            const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t write(
            const chromatic_name_service & instance_data,
            ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t write_w_timestamp(
            const chromatic_name_service & instance_data,
            ::DDS::InstanceHandle_t handle,
            const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t dispose(
            const chromatic_name_service & instance_data,
            ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t dispose_w_timestamp(
            const chromatic_name_service & instance_data,
            ::DDS::InstanceHandle_t handle,
            const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t writedispose(
            const chromatic_name_service & instance_data,
            ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t writedispose_w_timestamp(
            const chromatic_name_service & instance_data,
            ::DDS::InstanceHandle_t handle,
            const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t get_key_value(
            chromatic_name_service & key_holder,
            ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::InstanceHandle_t lookup_instance(
            const chromatic_name_service & instance_data) THROW_ORB_EXCEPTIONS;
    
    
        chromatic_name_serviceDataWriter_impl (
            gapi_dataWriter handle
        );
    
        virtual ~chromatic_name_serviceDataWriter_impl (void);
    
    private:
        chromatic_name_serviceDataWriter_impl(const chromatic_name_serviceDataWriter_impl &);
        void operator= (const chromatic_name_serviceDataWriter &);
    };
    
    class  chromatic_name_serviceDataReader_impl : public virtual chromatic_name_serviceDataReader,
                                        public ::DDS::DataReader_impl
    {
        friend class chromatic_name_serviceDataReaderView_impl;
    public:
        virtual ::DDS::ReturnCode_t read(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_w_condition(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_w_condition(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_next_sample(
            chromatic_name_service & received_data,
            ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_next_sample(
            chromatic_name_service & received_data,
            ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_instance(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_instance(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_next_instance(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_next_instance(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t return_loan(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t get_key_value(
            chromatic_name_service & key_holder,
            ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::InstanceHandle_t lookup_instance(
            const chromatic_name_service & instance) THROW_ORB_EXCEPTIONS;
    
        chromatic_name_serviceDataReader_impl (
            gapi_dataReader handle
        );
    
        virtual ~chromatic_name_serviceDataReader_impl(void);
    
    private:
        chromatic_name_serviceDataReader_impl(const chromatic_name_serviceDataReader &);
        void operator= (const chromatic_name_serviceDataReader &);
    
        static ::DDS::ReturnCode_t check_preconditions(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples
        );
    };
    
    class  chromatic_name_serviceDataReaderView_impl : public virtual chromatic_name_serviceDataReaderView,
                                        public ::DDS::DataReaderView_impl
    {
    public:
        virtual ::DDS::ReturnCode_t read(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_w_condition(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_w_condition(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_next_sample(
            chromatic_name_service & received_data,
            ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_next_sample(
            chromatic_name_service & received_data,
            ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_instance(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_instance(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_next_instance(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_next_instance(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            CORBA::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t return_loan(
            chromatic_name_serviceSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::ReturnCode_t get_key_value(
            chromatic_name_service & key_holder,
            ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
    
        virtual ::DDS::InstanceHandle_t lookup_instance(
            const chromatic_name_service & instance) THROW_ORB_EXCEPTIONS;
    
        chromatic_name_serviceDataReaderView_impl (
            gapi_dataReader handle
        );
    
        virtual ~chromatic_name_serviceDataReaderView_impl(void);
    
    private:
        chromatic_name_serviceDataReaderView_impl(const chromatic_name_serviceDataReaderView &);
        void operator= (const chromatic_name_serviceDataReaderView &);
    };
    
}

#endif
