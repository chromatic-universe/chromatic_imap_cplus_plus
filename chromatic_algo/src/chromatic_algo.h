/*
 * chromatic_algo.h
 *
 *  Created on: Sep 30, 2014
 *      Author: wiljoh
 */

#ifndef CHROMATIC_ALGO_H_
#define CHROMATIC_ALGO_H_

#include <iostream>

namespace chromatic_algorithm
{
	//forward
	class chromatic_node;
	class chromatic_node_ex;

	//enumerations

	//typedefs
	typedef chromatic_node* chromatic_node_ptr;
	typedef chromatic_node_ex* chromatic_node_ex_ptr;

	//classes
	//--------------------------------------------------------------------------------------------------
	class chromatic_node
	{

		public:

			//ctors
			chromatic_node( int data , chromatic_node_ptr ptr_node = nullptr );
			explicit chromatic_node();

			//dtor
			virtual ~chromatic_node();

		private:

			//

		protected:

			//attributes
			int m_dwData;
			chromatic_node_ptr m_ptr_next;

		public:

			//accessors
			inline int data() const noexcept { return m_dwData; }
			chromatic_node_ptr next() { return m_ptr_next; }

			//mutators
			inline void data( int dw_data ) noexcept { m_dwData = dw_data; }  ;
			void next( chromatic_node_ptr ptr_node ) noexcept { m_ptr_next = ptr_node; }

	};

	//--------------------------------------------------------------------------------------------------
	class chromatic_node_ex : public chromatic_node
	{

		public:

			//ctors
		    chromatic_node_ex( int data ,
		    		           chromatic_node_ex_ptr ptr_previous = nullptr ,
		    		           chromatic_node_ex_ptr ptr_next = nullptr);
		    explicit chromatic_node_ex();

		    //dtor
		    virtual ~chromatic_node_ex();

		protected:

		    //attributes
		    chromatic_node_ex_ptr m_ptr_ex_previous;
		    chromatic_node_ex_ptr m_ptr_ex_next;

		public:

		    //accessors
			chromatic_node_ex_ptr next() { return m_ptr_ex_next; }

			//mutators
			void next( chromatic_node_ex_ptr ptr_node ) noexcept { m_ptr_ex_next = ptr_node; }

	};


	//--------------------------------------------------------------------------------------------------
	class chromatic_single_link
	{

		public:

			//ctor
			explicit chromatic_single_link();

			//dtor
			virtual ~chromatic_single_link();

		protected:

			//attributes
			chromatic_node_ptr m_ptr_head;

		public:

			//services
			virtual void append( int data );
			virtual void remove( int data );


	};

	//--------------------------------------------------------------------------------------------------
	class chromatic_double_link
	{

		public:

			//ctor
			explicit chromatic_double_link();

			//dtor
			virtual ~chromatic_double_link();

		protected:

			//attributes
			chromatic_node_ptr m_ptr_ex_front;
			chromatic_node_ptr m_ptr_ex_back;

			//services
			void push_front( int data );
			void push_back( int data );

	};


}


#endif /* CHROMATIC_ALGO_H_ */
