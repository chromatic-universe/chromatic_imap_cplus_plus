#include "chromatic_imap_ddsDcps_impl.h"
#include "gapi.h"
#include "gapi_loanRegistry.h"
#include "chromatic_imap_ddsSplDcps.h"
#include "ccpp_DataReader_impl.h"
#include "ccpp_DataReaderView_impl.h"


extern c_bool
__chromatic_imap_dds_chromatic_imap_command__copyIn(
    c_base base,
    struct chromatic_imap_dds::chromatic_imap_command *from,
    struct _chromatic_imap_dds_chromatic_imap_command *to);

extern void
__chromatic_imap_dds_chromatic_imap_command__copyOut(
    void *_from,
    void *_to);

// DDS chromatic_imap_dds::chromatic_imap_command TypeSupportFactory Object Body

::DDS::DataWriter_ptr
chromatic_imap_dds::chromatic_imap_commandTypeSupportFactory::create_datawriter (gapi_dataWriter handle)
{
    return new chromatic_imap_dds::chromatic_imap_commandDataWriter_impl(handle);
}

::DDS::DataReader_ptr
chromatic_imap_dds::chromatic_imap_commandTypeSupportFactory::create_datareader (gapi_dataReader handle)
{
    return new chromatic_imap_dds::chromatic_imap_commandDataReader_impl (handle);
}


::DDS::DataReaderView_ptr
chromatic_imap_dds::chromatic_imap_commandTypeSupportFactory::create_view (gapi_dataReaderView handle)
{
    return new chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl (handle);
}

// DDS chromatic_imap_dds::chromatic_imap_command TypeSupport Object Body

chromatic_imap_dds::chromatic_imap_commandTypeSupport::chromatic_imap_commandTypeSupport(void) :
    TypeSupport_impl(
        __chromatic_imap_dds_chromatic_imap_command__name(),
        __chromatic_imap_dds_chromatic_imap_command__keys(),
        chromatic_imap_dds::chromatic_imap_commandTypeSupport::metaDescriptor,
        (gapi_copyIn) __chromatic_imap_dds_chromatic_imap_command__copyIn,
        (gapi_copyOut) __chromatic_imap_dds_chromatic_imap_command__copyOut,
        (gapi_readerCopy) ::DDS::ccpp_DataReaderCopy<chromatic_imap_dds::chromatic_imap_commandSeq, chromatic_imap_dds::chromatic_imap_command>,
        new  chromatic_imap_dds::chromatic_imap_commandTypeSupportFactory(),
        chromatic_imap_dds::chromatic_imap_commandTypeSupport::metaDescriptorArrLength)
{
    // Parent constructor takes care of everything.
}

chromatic_imap_dds::chromatic_imap_commandTypeSupport::~chromatic_imap_commandTypeSupport(void)
{
    // Parent destructor takes care of everything.
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandTypeSupport::register_type(
    ::DDS::DomainParticipant_ptr domain,
    const char * type_name) THROW_ORB_EXCEPTIONS
{
    return TypeSupport_impl::register_type(domain, type_name);
}

char *
chromatic_imap_dds::chromatic_imap_commandTypeSupport::get_type_name() THROW_ORB_EXCEPTIONS
{
    return TypeSupport_impl::get_type_name();
}

// DDS chromatic_imap_dds::chromatic_imap_command DataWriter_impl Object Body

chromatic_imap_dds::chromatic_imap_commandDataWriter_impl::chromatic_imap_commandDataWriter_impl (
    gapi_dataWriter handle
) : ::DDS::DataWriter_impl(handle)
{
    // Parent constructor takes care of everything.
}

chromatic_imap_dds::chromatic_imap_commandDataWriter_impl::~chromatic_imap_commandDataWriter_impl(void)
{
    // Parent destructor takes care of everything.
}

::DDS::InstanceHandle_t
chromatic_imap_dds::chromatic_imap_commandDataWriter_impl::register_instance(
    const chromatic_imap_dds::chromatic_imap_command & instance_data) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::register_instance(&instance_data);
}

::DDS::InstanceHandle_t
chromatic_imap_dds::chromatic_imap_commandDataWriter_impl::register_instance_w_timestamp(
    const chromatic_imap_command & instance_data,
    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::register_instance_w_timestamp(&instance_data, source_timestamp);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataWriter_impl::unregister_instance(
    const chromatic_imap_dds::chromatic_imap_command & instance_data,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::unregister_instance(&instance_data, handle);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataWriter_impl::unregister_instance_w_timestamp(
    const chromatic_imap_command & instance_data,
    ::DDS::InstanceHandle_t handle,
    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::unregister_instance_w_timestamp(&instance_data, handle, source_timestamp);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataWriter_impl::write(
    const chromatic_imap_dds::chromatic_imap_command & instance_data,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::write(&instance_data, handle);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataWriter_impl::write_w_timestamp(
    const chromatic_imap_command & instance_data,
    ::DDS::InstanceHandle_t handle,
    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::write_w_timestamp(&instance_data, handle, source_timestamp);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataWriter_impl::dispose(
    const chromatic_imap_dds::chromatic_imap_command & instance_data,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::dispose(&instance_data, handle);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataWriter_impl::dispose_w_timestamp(
    const chromatic_imap_command & instance_data,
    ::DDS::InstanceHandle_t handle,
    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::dispose_w_timestamp(&instance_data, handle, source_timestamp);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataWriter_impl::writedispose(
    const chromatic_imap_dds::chromatic_imap_command & instance_data,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::writedispose(&instance_data, handle);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataWriter_impl::writedispose_w_timestamp(
    const chromatic_imap_command & instance_data,
    ::DDS::InstanceHandle_t handle,
    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::writedispose_w_timestamp(&instance_data, handle, source_timestamp);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataWriter_impl::get_key_value(
    chromatic_imap_command & key_holder,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::get_key_value(&key_holder, handle);
}

::DDS::InstanceHandle_t
chromatic_imap_dds::chromatic_imap_commandDataWriter_impl::lookup_instance(
    const chromatic_imap_dds::chromatic_imap_command & instance_data) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::lookup_instance(&instance_data);
}

// DDS chromatic_imap_dds::chromatic_imap_command DataReader_impl Object Body

chromatic_imap_dds::chromatic_imap_commandDataReader_impl::chromatic_imap_commandDataReader_impl (
    gapi_dataReader handle
) : ::DDS::DataReader_impl(handle, ::DDS::ccpp_DataReaderParallelDemarshallingMain<chromatic_imap_dds::chromatic_imap_commandSeq>)
{
    // Parent constructor takes care of everything.
}

chromatic_imap_dds::chromatic_imap_commandDataReader_impl::~chromatic_imap_commandDataReader_impl(void)
{
    // Parent destructor takes care of everything.
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReader_impl::read(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::read(&received_data, info_seq, max_samples, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReader_impl::take(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::take(&received_data, info_seq, max_samples, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReader_impl::read_w_condition(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::read_w_condition(&received_data, info_seq, max_samples, a_condition);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReader_impl::take_w_condition(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::take_w_condition(&received_data, info_seq, max_samples, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReader_impl::read_next_sample(
    chromatic_imap_dds::chromatic_imap_command & received_data,
    ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS
{
    return DataReader_impl::read_next_sample(&received_data, sample_info);
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReader_impl::take_next_sample(
    chromatic_imap_dds::chromatic_imap_command & received_data,
    ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS
{
    return DataReader_impl::take_next_sample(&received_data, sample_info);
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReader_impl::read_instance(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::read_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReader_impl::take_instance(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::take_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReader_impl::read_next_instance(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::read_next_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReader_impl::take_next_instance(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::take_next_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReader_impl::read_next_instance_w_condition(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::read_next_instance_w_condition(&received_data, info_seq, max_samples, a_handle, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReader_impl::take_next_instance_w_condition(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::take_next_instance_w_condition(&received_data, info_seq, max_samples, a_handle, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReader_impl::return_loan(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status = ::DDS::RETCODE_OK;

    if ( received_data.length() > 0 ) {
        if (received_data.length() == info_seq.length() &&
            received_data.release() == info_seq.release() ) {
            if (!received_data.release()) {
                status = DataReader_impl::return_loan( received_data.get_buffer(),
                                                       info_seq.get_buffer() );

                if ( status == ::DDS::RETCODE_OK ) {
                    if ( !received_data.release() ) {
                        chromatic_imap_dds::chromatic_imap_commandSeq::freebuf( received_data.get_buffer(false) );
                        received_data.replace(0, 0, NULL, false);
                        ::DDS::SampleInfoSeq::freebuf( info_seq.get_buffer(false) );
                        info_seq.replace(0, 0, NULL, false);
                    }
                } else if ( status == ::DDS::RETCODE_NO_DATA ) {
                    if ( received_data.release() ) {
                        status = ::DDS::RETCODE_OK;
                    } else {
                        status = ::DDS::RETCODE_PRECONDITION_NOT_MET;
                    }
                }
            }
        } else {
            status = ::DDS::RETCODE_PRECONDITION_NOT_MET;
        }
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReader_impl::get_key_value(
    chromatic_imap_dds::chromatic_imap_command & key_holder,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataReader_impl::get_key_value(&key_holder, handle);
}

::DDS::InstanceHandle_t
chromatic_imap_dds::chromatic_imap_commandDataReader_impl::lookup_instance(
    const chromatic_imap_dds::chromatic_imap_command & instance) THROW_ORB_EXCEPTIONS
{
    return DataReader_impl::lookup_instance(&instance);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReader_impl::check_preconditions(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples)
{
    ::DDS::ReturnCode_t status = ::DDS::RETCODE_PRECONDITION_NOT_MET;

    if ( received_data.length() == info_seq.length() &&
         received_data.maximum() == info_seq.maximum() &&
         received_data.release() == info_seq.release() ) {
        if ( received_data.maximum() == 0 || received_data.release() ) {
            if (received_data.maximum() == 0 ||
                max_samples <= static_cast<CORBA::Long>(received_data.maximum()) ||
                max_samples == ::DDS::LENGTH_UNLIMITED ) {
                status = ::DDS::RETCODE_OK;
            }
        }
    }
    return status;
}


// DDS chromatic_imap_dds::chromatic_imap_command DataReaderView_impl Object Body

chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::chromatic_imap_commandDataReaderView_impl (
    gapi_dataReaderView handle
) : ::DDS::DataReaderView_impl(handle)
{
    // Parent constructor takes care of everything.
}

chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::~chromatic_imap_commandDataReaderView_impl(void)
{
    // Parent destructor takes care of everything.
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::read(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_imap_commandDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::read(&received_data, info_seq, max_samples, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::take(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_imap_commandDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::take(&received_data, info_seq, max_samples, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::read_w_condition(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_imap_commandDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::read_w_condition(&received_data, info_seq, max_samples, a_condition);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::take_w_condition(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_imap_commandDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::take_w_condition(&received_data, info_seq, max_samples, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::read_next_sample(
    chromatic_imap_dds::chromatic_imap_command & received_data,
    ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS
{
    return DataReaderView_impl::read_next_sample(&received_data, sample_info);
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::take_next_sample(
    chromatic_imap_dds::chromatic_imap_command & received_data,
    ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS
{
    return DataReaderView_impl::take_next_sample(&received_data, sample_info);
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::read_instance(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_imap_commandDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::read_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::take_instance(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_imap_commandDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::take_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::read_next_instance(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_imap_commandDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::read_next_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::take_next_instance(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_imap_commandDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::take_next_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::read_next_instance_w_condition(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_imap_commandDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::read_next_instance_w_condition(&received_data, info_seq, max_samples, a_handle, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::take_next_instance_w_condition(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_imap_commandDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::take_next_instance_w_condition(&received_data, info_seq, max_samples, a_handle, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::return_loan(
    chromatic_imap_dds::chromatic_imap_commandSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status = ::DDS::RETCODE_OK;

    if ( received_data.length() > 0 ) {
        if (received_data.length() == info_seq.length() &&
            received_data.release() == info_seq.release() ) {
            if (!received_data.release()) {
                status = DataReaderView_impl::return_loan( received_data.get_buffer(),
                                                       info_seq.get_buffer() );

                if ( status == ::DDS::RETCODE_OK ) {
                    if ( !received_data.release() ) {
                        chromatic_imap_dds::chromatic_imap_commandSeq::freebuf( received_data.get_buffer(false) );
                        received_data.replace(0, 0, NULL, false);
                        ::DDS::SampleInfoSeq::freebuf( info_seq.get_buffer(false) );
                        info_seq.replace(0, 0, NULL, false);
                    }
                } else if ( status == ::DDS::RETCODE_NO_DATA ) {
                    if ( received_data.release() ) {
                        status = ::DDS::RETCODE_OK;
                    } else {
                        status = ::DDS::RETCODE_PRECONDITION_NOT_MET;
                    }
                }
            }
        } else {
            status = ::DDS::RETCODE_PRECONDITION_NOT_MET;
        }
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::get_key_value(
    chromatic_imap_dds::chromatic_imap_command & key_holder,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataReaderView_impl::get_key_value(&key_holder, handle);
}

::DDS::InstanceHandle_t
chromatic_imap_dds::chromatic_imap_commandDataReaderView_impl::lookup_instance(
    const chromatic_imap_dds::chromatic_imap_command & instance) THROW_ORB_EXCEPTIONS
{
    return DataReaderView_impl::lookup_instance(&instance);
}



extern c_bool
__chromatic_imap_dds_chromatic_name_service__copyIn(
    c_base base,
    struct chromatic_imap_dds::chromatic_name_service *from,
    struct _chromatic_imap_dds_chromatic_name_service *to);

extern void
__chromatic_imap_dds_chromatic_name_service__copyOut(
    void *_from,
    void *_to);

// DDS chromatic_imap_dds::chromatic_name_service TypeSupportFactory Object Body

::DDS::DataWriter_ptr
chromatic_imap_dds::chromatic_name_serviceTypeSupportFactory::create_datawriter (gapi_dataWriter handle)
{
    return new chromatic_imap_dds::chromatic_name_serviceDataWriter_impl(handle);
}

::DDS::DataReader_ptr
chromatic_imap_dds::chromatic_name_serviceTypeSupportFactory::create_datareader (gapi_dataReader handle)
{
    return new chromatic_imap_dds::chromatic_name_serviceDataReader_impl (handle);
}


::DDS::DataReaderView_ptr
chromatic_imap_dds::chromatic_name_serviceTypeSupportFactory::create_view (gapi_dataReaderView handle)
{
    return new chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl (handle);
}

// DDS chromatic_imap_dds::chromatic_name_service TypeSupport Object Body

chromatic_imap_dds::chromatic_name_serviceTypeSupport::chromatic_name_serviceTypeSupport(void) :
    TypeSupport_impl(
        __chromatic_imap_dds_chromatic_name_service__name(),
        __chromatic_imap_dds_chromatic_name_service__keys(),
        chromatic_imap_dds::chromatic_name_serviceTypeSupport::metaDescriptor,
        (gapi_copyIn) __chromatic_imap_dds_chromatic_name_service__copyIn,
        (gapi_copyOut) __chromatic_imap_dds_chromatic_name_service__copyOut,
        (gapi_readerCopy) ::DDS::ccpp_DataReaderCopy<chromatic_imap_dds::chromatic_name_serviceSeq, chromatic_imap_dds::chromatic_name_service>,
        new  chromatic_imap_dds::chromatic_name_serviceTypeSupportFactory(),
        chromatic_imap_dds::chromatic_name_serviceTypeSupport::metaDescriptorArrLength)
{
    // Parent constructor takes care of everything.
}

chromatic_imap_dds::chromatic_name_serviceTypeSupport::~chromatic_name_serviceTypeSupport(void)
{
    // Parent destructor takes care of everything.
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceTypeSupport::register_type(
    ::DDS::DomainParticipant_ptr domain,
    const char * type_name) THROW_ORB_EXCEPTIONS
{
    return TypeSupport_impl::register_type(domain, type_name);
}

char *
chromatic_imap_dds::chromatic_name_serviceTypeSupport::get_type_name() THROW_ORB_EXCEPTIONS
{
    return TypeSupport_impl::get_type_name();
}

// DDS chromatic_imap_dds::chromatic_name_service DataWriter_impl Object Body

chromatic_imap_dds::chromatic_name_serviceDataWriter_impl::chromatic_name_serviceDataWriter_impl (
    gapi_dataWriter handle
) : ::DDS::DataWriter_impl(handle)
{
    // Parent constructor takes care of everything.
}

chromatic_imap_dds::chromatic_name_serviceDataWriter_impl::~chromatic_name_serviceDataWriter_impl(void)
{
    // Parent destructor takes care of everything.
}

::DDS::InstanceHandle_t
chromatic_imap_dds::chromatic_name_serviceDataWriter_impl::register_instance(
    const chromatic_imap_dds::chromatic_name_service & instance_data) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::register_instance(&instance_data);
}

::DDS::InstanceHandle_t
chromatic_imap_dds::chromatic_name_serviceDataWriter_impl::register_instance_w_timestamp(
    const chromatic_name_service & instance_data,
    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::register_instance_w_timestamp(&instance_data, source_timestamp);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataWriter_impl::unregister_instance(
    const chromatic_imap_dds::chromatic_name_service & instance_data,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::unregister_instance(&instance_data, handle);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataWriter_impl::unregister_instance_w_timestamp(
    const chromatic_name_service & instance_data,
    ::DDS::InstanceHandle_t handle,
    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::unregister_instance_w_timestamp(&instance_data, handle, source_timestamp);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataWriter_impl::write(
    const chromatic_imap_dds::chromatic_name_service & instance_data,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::write(&instance_data, handle);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataWriter_impl::write_w_timestamp(
    const chromatic_name_service & instance_data,
    ::DDS::InstanceHandle_t handle,
    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::write_w_timestamp(&instance_data, handle, source_timestamp);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataWriter_impl::dispose(
    const chromatic_imap_dds::chromatic_name_service & instance_data,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::dispose(&instance_data, handle);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataWriter_impl::dispose_w_timestamp(
    const chromatic_name_service & instance_data,
    ::DDS::InstanceHandle_t handle,
    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::dispose_w_timestamp(&instance_data, handle, source_timestamp);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataWriter_impl::writedispose(
    const chromatic_imap_dds::chromatic_name_service & instance_data,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::writedispose(&instance_data, handle);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataWriter_impl::writedispose_w_timestamp(
    const chromatic_name_service & instance_data,
    ::DDS::InstanceHandle_t handle,
    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::writedispose_w_timestamp(&instance_data, handle, source_timestamp);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataWriter_impl::get_key_value(
    chromatic_name_service & key_holder,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::get_key_value(&key_holder, handle);
}

::DDS::InstanceHandle_t
chromatic_imap_dds::chromatic_name_serviceDataWriter_impl::lookup_instance(
    const chromatic_imap_dds::chromatic_name_service & instance_data) THROW_ORB_EXCEPTIONS
{
    return DataWriter_impl::lookup_instance(&instance_data);
}

// DDS chromatic_imap_dds::chromatic_name_service DataReader_impl Object Body

chromatic_imap_dds::chromatic_name_serviceDataReader_impl::chromatic_name_serviceDataReader_impl (
    gapi_dataReader handle
) : ::DDS::DataReader_impl(handle, ::DDS::ccpp_DataReaderParallelDemarshallingMain<chromatic_imap_dds::chromatic_name_serviceSeq>)
{
    // Parent constructor takes care of everything.
}

chromatic_imap_dds::chromatic_name_serviceDataReader_impl::~chromatic_name_serviceDataReader_impl(void)
{
    // Parent destructor takes care of everything.
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReader_impl::read(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::read(&received_data, info_seq, max_samples, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReader_impl::take(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::take(&received_data, info_seq, max_samples, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReader_impl::read_w_condition(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::read_w_condition(&received_data, info_seq, max_samples, a_condition);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReader_impl::take_w_condition(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::take_w_condition(&received_data, info_seq, max_samples, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReader_impl::read_next_sample(
    chromatic_imap_dds::chromatic_name_service & received_data,
    ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS
{
    return DataReader_impl::read_next_sample(&received_data, sample_info);
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReader_impl::take_next_sample(
    chromatic_imap_dds::chromatic_name_service & received_data,
    ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS
{
    return DataReader_impl::take_next_sample(&received_data, sample_info);
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReader_impl::read_instance(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::read_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReader_impl::take_instance(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::take_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReader_impl::read_next_instance(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::read_next_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReader_impl::take_next_instance(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::take_next_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReader_impl::read_next_instance_w_condition(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::read_next_instance_w_condition(&received_data, info_seq, max_samples, a_handle, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReader_impl::take_next_instance_w_condition(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReader_impl::take_next_instance_w_condition(&received_data, info_seq, max_samples, a_handle, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReader_impl::return_loan(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status = ::DDS::RETCODE_OK;

    if ( received_data.length() > 0 ) {
        if (received_data.length() == info_seq.length() &&
            received_data.release() == info_seq.release() ) {
            if (!received_data.release()) {
                status = DataReader_impl::return_loan( received_data.get_buffer(),
                                                       info_seq.get_buffer() );

                if ( status == ::DDS::RETCODE_OK ) {
                    if ( !received_data.release() ) {
                        chromatic_imap_dds::chromatic_name_serviceSeq::freebuf( received_data.get_buffer(false) );
                        received_data.replace(0, 0, NULL, false);
                        ::DDS::SampleInfoSeq::freebuf( info_seq.get_buffer(false) );
                        info_seq.replace(0, 0, NULL, false);
                    }
                } else if ( status == ::DDS::RETCODE_NO_DATA ) {
                    if ( received_data.release() ) {
                        status = ::DDS::RETCODE_OK;
                    } else {
                        status = ::DDS::RETCODE_PRECONDITION_NOT_MET;
                    }
                }
            }
        } else {
            status = ::DDS::RETCODE_PRECONDITION_NOT_MET;
        }
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReader_impl::get_key_value(
    chromatic_imap_dds::chromatic_name_service & key_holder,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataReader_impl::get_key_value(&key_holder, handle);
}

::DDS::InstanceHandle_t
chromatic_imap_dds::chromatic_name_serviceDataReader_impl::lookup_instance(
    const chromatic_imap_dds::chromatic_name_service & instance) THROW_ORB_EXCEPTIONS
{
    return DataReader_impl::lookup_instance(&instance);
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReader_impl::check_preconditions(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples)
{
    ::DDS::ReturnCode_t status = ::DDS::RETCODE_PRECONDITION_NOT_MET;

    if ( received_data.length() == info_seq.length() &&
         received_data.maximum() == info_seq.maximum() &&
         received_data.release() == info_seq.release() ) {
        if ( received_data.maximum() == 0 || received_data.release() ) {
            if (received_data.maximum() == 0 ||
                max_samples <= static_cast<CORBA::Long>(received_data.maximum()) ||
                max_samples == ::DDS::LENGTH_UNLIMITED ) {
                status = ::DDS::RETCODE_OK;
            }
        }
    }
    return status;
}


// DDS chromatic_imap_dds::chromatic_name_service DataReaderView_impl Object Body

chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::chromatic_name_serviceDataReaderView_impl (
    gapi_dataReaderView handle
) : ::DDS::DataReaderView_impl(handle)
{
    // Parent constructor takes care of everything.
}

chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::~chromatic_name_serviceDataReaderView_impl(void)
{
    // Parent destructor takes care of everything.
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::read(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_name_serviceDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::read(&received_data, info_seq, max_samples, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::take(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_name_serviceDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::take(&received_data, info_seq, max_samples, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::read_w_condition(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_name_serviceDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::read_w_condition(&received_data, info_seq, max_samples, a_condition);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::take_w_condition(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_name_serviceDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::take_w_condition(&received_data, info_seq, max_samples, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::read_next_sample(
    chromatic_imap_dds::chromatic_name_service & received_data,
    ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS
{
    return DataReaderView_impl::read_next_sample(&received_data, sample_info);
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::take_next_sample(
    chromatic_imap_dds::chromatic_name_service & received_data,
    ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS
{
    return DataReaderView_impl::take_next_sample(&received_data, sample_info);
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::read_instance(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_name_serviceDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::read_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::take_instance(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_name_serviceDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::take_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::read_next_instance(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_name_serviceDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::read_next_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}

::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::take_next_instance(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::SampleStateMask sample_states,
    ::DDS::ViewStateMask view_states,
    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_name_serviceDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::take_next_instance(&received_data, info_seq, max_samples, a_handle, sample_states, view_states, instance_states);
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::read_next_instance_w_condition(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_name_serviceDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::read_next_instance_w_condition(&received_data, info_seq, max_samples, a_handle, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::take_next_instance_w_condition(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq,
    CORBA::Long max_samples,
    ::DDS::InstanceHandle_t a_handle,
    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status;

    status = chromatic_imap_dds::chromatic_name_serviceDataReader_impl::check_preconditions(received_data, info_seq, max_samples);
    if ( status == ::DDS::RETCODE_OK ) {
        status = DataReaderView_impl::take_next_instance_w_condition(&received_data, info_seq, max_samples, a_handle, a_condition);
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::return_loan(
    chromatic_imap_dds::chromatic_name_serviceSeq & received_data,
    ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS
{
    ::DDS::ReturnCode_t status = ::DDS::RETCODE_OK;

    if ( received_data.length() > 0 ) {
        if (received_data.length() == info_seq.length() &&
            received_data.release() == info_seq.release() ) {
            if (!received_data.release()) {
                status = DataReaderView_impl::return_loan( received_data.get_buffer(),
                                                       info_seq.get_buffer() );

                if ( status == ::DDS::RETCODE_OK ) {
                    if ( !received_data.release() ) {
                        chromatic_imap_dds::chromatic_name_serviceSeq::freebuf( received_data.get_buffer(false) );
                        received_data.replace(0, 0, NULL, false);
                        ::DDS::SampleInfoSeq::freebuf( info_seq.get_buffer(false) );
                        info_seq.replace(0, 0, NULL, false);
                    }
                } else if ( status == ::DDS::RETCODE_NO_DATA ) {
                    if ( received_data.release() ) {
                        status = ::DDS::RETCODE_OK;
                    } else {
                        status = ::DDS::RETCODE_PRECONDITION_NOT_MET;
                    }
                }
            }
        } else {
            status = ::DDS::RETCODE_PRECONDITION_NOT_MET;
        }
    }
    return status;
}


::DDS::ReturnCode_t
chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::get_key_value(
    chromatic_imap_dds::chromatic_name_service & key_holder,
    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS
{
    return DataReaderView_impl::get_key_value(&key_holder, handle);
}

::DDS::InstanceHandle_t
chromatic_imap_dds::chromatic_name_serviceDataReaderView_impl::lookup_instance(
    const chromatic_imap_dds::chromatic_name_service & instance) THROW_ORB_EXCEPTIONS
{
    return DataReaderView_impl::lookup_instance(&instance);
}



const char * ::chromatic_imap_dds::chromatic_imap_commandTypeSupport::metaDescriptor[] = {"<MetaData version=\"1.0.0\"><Module name=\"chromatic_imap_dds\"><TypeDef name=\"command_type\"><String length=\"32\"/>",
"</TypeDef><Struct name=\"chromatic_imap_command\"><Member name=\"session_idx\"><Long/></Member><Member name=\"locale_idx\">",
"<Long/></Member><Member name=\"moniker\"><String/></Member><Member name=\"command\"><Type name=\"command_type\"/>",
"</Member><Member name=\"payload\"><String/></Member></Struct></Module></MetaData>"};
const CORBA::ULong (::chromatic_imap_dds::chromatic_imap_commandTypeSupport::metaDescriptorArrLength) = 4;
const char * ::chromatic_imap_dds::chromatic_name_serviceTypeSupport::metaDescriptor[] = {"<MetaData version=\"1.0.0\"><Module name=\"chromatic_imap_dds\"><TypeDef name=\"user_id\"><String length=\"32\"/>",
"</TypeDef><Struct name=\"chromatic_name_service\"><Member name=\"session_idx\"><Long/></Member><Member name=\"user_name\">",
"<Type name=\"user_id\"/></Member></Struct></Module></MetaData>"};
const CORBA::ULong (::chromatic_imap_dds::chromatic_name_serviceTypeSupport::metaDescriptorArrLength) = 3;
