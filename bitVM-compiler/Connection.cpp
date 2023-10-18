// implementation of class Connection

#include "Connection.h"
#include "Gate.h"

bool Connection::is_calculated(void) const {
	return bit_value != nullptr; 
}
// get the value of the bit
bool Connection::get_value(void) const {
	assert(bit_value != nullptr);
	return *bit_value;
}
// set the value of the bit
void Connection::set_value(bool b) {
	assert(bit_value == nullptr);
	bit_value = new Bit();
	bit_value->set_value(b);
}
// rest the value of the bit for a new run
void Connection::reset(void) {
	delete bit_value;
	bit_value = nullptr;
}