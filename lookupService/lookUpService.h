/*
 * lookUpService.h
 *
 *  Created on: Oct 11, 2015
 *      Author: root
 */

#ifndef LOOKUPSERVICE_H_
#define LOOKUPSERVICE_H_

#include <string.h>
#include <hiredis.h>

static const int WISER = 555;
static const char* HOSTNAME = "127.0.0.1";
static const int PORT = 6379;
static const int DISABLE = 0; //disables lus actions, quagga runs normally as if this were not here

static redisContext* context;

//structrue of integrated advertisement
//leaving path and asdescriptors out for the time being
struct integratedAdvert{
	struct pathDescriptors* pathAttributes;
	struct pathDescriptors* nodeAttributes;
	struct pathDescriptors* edgeAttributes; //not done yet, may need some thinking
};


//holds the path attritubtes for each path
struct pathDescriptors{
	struct pathDescriptors* next; //next descriptor associated with another path
	struct protoFields* fields; //the protocols and attributes associated with this path
	int numProtoFields; //number of protoocols that have attributes for this path
	int id; //the id for the path, node, or edge


};

//structure that holds the fields associated with a protocol
struct protoFields{
	struct field* fields;
	int numFields;
	int protoID; //number signifying what protocol is owner of these fields
};

//contains the essentially byte array associated with a field
struct field{
	char* fieldName;
	char* aFieldValue;
	int fieldValSize;
	int fieldNameSize;
};

extern int lus_initContext();

extern int lus_setIA(char* key, struct integratedAdvert);

extern struct integratedAdvert lus_getIA(char* key);

extern void lus_initIntegratedAdvert(struct integratedAdvert* advert);

extern redisContext* lus_getContext();


#endif /* LOOKUPSERVICE_H_ */
