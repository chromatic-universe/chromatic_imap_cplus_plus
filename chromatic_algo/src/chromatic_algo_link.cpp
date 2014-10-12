/*
 * chromatic_algo.cpp
 *
 *  Created on: Sep 30, 2014
 *      Author: wiljoh
 */

#include "chromatic_algo.h"

using std::cout;
using std::endl;

using namespace chromatic_algorithm;

//--------------------------------------------------------------------------------------------------
chromatic_node::chromatic_node() : m_dwData {} ,
								   m_ptr_next { nullptr }
{

}

//--------------------------------------------------------------------------------------------------
chromatic_node::chromatic_node( int data , chromatic_node_ptr ptr_node ) : m_dwData { data } ,
								   	   	   	   	   	   	   	   	   	   	   m_ptr_next { ptr_node }
{

}

//--------------------------------------------------------------------------------------------------
chromatic_node::~chromatic_node()
{
	//
}


//--------------------------------------------------------------------------------------------------
chromatic_single_link::chromatic_single_link() : m_ptr_head{ nullptr }
{

}

//--------------------------------------------------------------------------------------------------
chromatic_single_link::~chromatic_single_link()
{

}

//--------------------------------------------------------------------------------------------------
void chromatic_single_link::append( int data )
{
		try
		{
			//create a new node
			chromatic_node_ptr new_node = new chromatic_node( data , nullptr );

			//create a temporary pointer
			chromatic_node_ptr temp_node = m_ptr_head;

			if( temp_node != nullptr )
			{
				//nodes already present in list , iterate to end
				while( temp_node->next() != nullptr )
				{
					temp_node = temp_node->next();
				}

				//point the last node to the new node
				temp_node->next( new_node );
			}
			else
			{
				//first node in the list
				m_ptr_head = new_node;

			}


		}
		catch( ... )
		{

		}
}

//--------------------------------------------------------------------------------------------------
void chromatic_single_link::remove( int data )
{
		try
		{
			//create a temporary pointer
			chromatic_node_ptr temp_node = m_ptr_head;

			//if nodes
			if( temp_node != nullptr )
			{
				//last node of the list
				if( temp_node->next() == nullptr )
				{
					delete temp_node;
					m_ptr_head = nullptr;
				}
				else
				{
					//parse through the nodes
					chromatic_node_ptr prev;
					do
					{
						if( temp_node->data() == data )
						{
							break;
						}
						prev = temp_node;
						temp_node = temp_node->next();
					}
					while ( temp_node != nullptr );

					//adjust the pointers
					prev->next( temp_node->next() );

					//delete the current node
					delete temp_node;
				}
			}
		}
		catch( ... )
		{

		}
}

//--------------------------------------------------------------------------------------------------
chromatic_node_ex::chromatic_node_ex( int data ,
						  	 	 	  chromatic_node_ex_ptr ptr_previous ,
						  	 	 	  chromatic_node_ex_ptr ptr_next ) :  m_ptr_ex_previous { ptr_previous } ,
	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	  m_ptr_ex_next  { ptr_next }
{
		chromatic_node::data( data );
}

//--------------------------------------------------------------------------------------------------
chromatic_node_ex::chromatic_node_ex()
{
	//
}

//--------------------------------------------------------------------------------------------------
chromatic_node_ex::~chromatic_node_ex()
{
	//
}



//--------------------------------------------------------------------------------------------------
chromatic_double_link::chromatic_double_link()
{

}

//--------------------------------------------------------------------------------------------------
chromatic_double_link::~chromatic_double_link()
{

}

//--------------------------------------------------------------------------------------------------
void chromatic_double_link::push_front( int data )
{

}

//--------------------------------------------------------------------------------------------------
void chromatic_double_link::push_back( int data )
{
		try
		{
			chromatic_node_ex_ptr node = new chromatic_node_ex( data );
			//if( _m_ptr_ex_prev_)
		}
		catch( ... )
		{

		}
}
