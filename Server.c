#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include "open62541.h"


//Define the sampling time for the sensor
#define SLEEP_TIME_MILLIS 50

//Define the ID of the node externally as it will be needed inside the thread
#define COUNTER_NODE_ID 20305

UA_Boolean running = true;

int32_t numberOfParts = 0;
int32_t upper=50,lower=25;

static void stopHandler(int sig) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Recibido ctrl-c");
    running = false;
}


//Thread used to simulate Temp sensor value
void *mainSensor(void *ptr) {
	UA_Server * server = ptr;
	int utime = SLEEP_TIME_MILLIS * 1000;
	
	bool valueIsZero = false;
	
	//Temperature sensor value simulated 
	while (running == 1){
		
		numberOfParts = (rand() %  (upper - lower + 1)) + lower;
		
		//Update the OPC-UA node
		UA_Variant value;
		UA_Int32 myInteger = (UA_Int32) numberOfParts;
		UA_Variant_setScalarCopy(&value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
		UA_Server_writeValue(server, UA_NODEID_NUMERIC(1,COUNTER_NODE_ID), value);
				
		usleep(utime);
	}
}

static void addCounterSensorVariable(UA_Server * server) {

	UA_NodeId counterNodeId = UA_NODEID_NUMERIC(1, COUNTER_NODE_ID);
	UA_QualifiedName counterName = UA_QUALIFIEDNAME(1, "Piece Counter[pieces]");
	UA_VariableAttributes attr = UA_VariableAttributes_default;

	attr.description = UA_LOCALIZEDTEXT("en_US","Piece Counter (units:pieces)");
	attr.displayName = UA_LOCALIZEDTEXT("en_US","Piece Counter");
  attr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;

	UA_Int32 counterValue = 0;

  UA_Variant_setScalarCopy(&attr.value, &counterValue, &UA_TYPES[UA_TYPES_INT32]);
	
	UA_Server_addVariableNode(server, counterNodeId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
		UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
		counterName, UA_NODEID_NULL, attr, NULL, NULL);
}

int main(void) {
	int ret;
	pthread_t threadSensor;
  signal(SIGINT, stopHandler);
  signal(SIGTERM, stopHandler);
    
  UA_ServerConfig *config = UA_ServerConfig_new_default();
  UA_Server *server = UA_Server_new(config);
	
		
  addCounterSensorVariable(server);
  //Launch the thread. The OPC-UA server is passed as parameter as the value of the node needs to be updated.
  if(pthread_create( &threadSensor, NULL, mainSensor, server)) {
  	fprintf(stderr,"Error - pthread_create(): %d\n",ret);
	exit(EXIT_FAILURE);
  }
	
  UA_StatusCode retval = UA_Server_run(server, &running);
  UA_Server_delete(server);
  UA_ServerConfig_delete(config);
  return (int)retval;
}
