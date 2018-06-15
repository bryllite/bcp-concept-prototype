#pragma once


// namespace bryllite
namespace bryllite {

	class finite_state_machine;

	// finite state class
	class finite_state
	{
		friend class finite_state_machine;
	protected:
		finite_state_machine& _fsm;

	public:
		finite_state(finite_state_machine& fsm);

		virtual ~finite_state();

		bool set_state( finite_state* state );

	protected:
		virtual void onStateInitialized( void );
		virtual void onStateFinalized( void );

		virtual int onStateBegin( void );
		virtual int onStateEnd(void);
		virtual int onStateRun(void);
	};

	// finite state machine class
	class finite_state_machine
	{
	protected:
		finite_state* _current_state;

	public:
		finite_state_machine();

		bool set_state( finite_state* state ); 
		finite_state* get_state( void );

		int process_state_main( void );
	};


};//namespace bryllite