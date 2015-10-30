/*
 * lookUpService.c
 *
 *  Created on: Oct 11, 2015
 *      Author: root
 */
#include "lookUpService.h"
static const int ARRLENGTH = 256;

int lus_initContext(){
	if(DISABLE){
		return 0;
	}
	struct timeval timeout = { 1, 500000 }; // 1.5 seconds
	context = redisConnectWithTimeout(HOSTNAME, PORT, timeout);
	if (context == NULL || context->err) {
		if (context) {
			printf("Connection error: %s\n", context->errstr);
			redisFree(context);
		} else {
			printf("Connection error: can't allocate redis context\n");
		}
		exit(1);
	}
	return 1;
}



//takes an integrated advertisement, a rediscontext (to senda nd receive) and puts in the lookup service
//key has to be pointing to a null terminated string
int lus_setIA(char* key, struct integratedAdvert advert)
{
	if(DISABLE)
	{
		return 0;
	}
	if(advert.pathAttributes != NULL)
	{
		struct pathDescriptors* pathAttribute = advert.pathAttributes;
		while(pathAttribute != NULL) //if there are path attributes
		{
			//for each protocol...
			for(int i = 0; i < pathAttribute->numProtoFields; i++)
			{
				//for each field associated with a protocol...
				struct protoFields protoField =  pathAttribute->fields[i];
				for(int j = 0; j < protoField.numFields; j++)
				{
					struct field aField = protoField.fields[j]; //grab the field using ptr aritthmmatic

/*					char value[ARRLENGTH]; //value we are sending
					memset (value, 0, sizeof(value)); //set to 0 for correctnes sake
					strcpy (value, key); //copy the key
					strcat (value, " ");
					strcat (value, aField.fieldName); //put the field name
					strcat (value, " ");
					strcat (value, aField.aFieldValue); //put the value

					printf("[debug] value is: %s\n", value);*/
					redisReply* reply;
					reply = redisCommand(context, "hmset %s %s %s", key, aField.fieldName, aField.aFieldValue); //send to the lookup service
					freeReplyObject(reply);
				}

			}
			pathAttribute = pathAttribute->next;
		}
	}
	//do the same for other fields, leaving that for later as we just want to get the peering experiments running
}

//will return a formed integrated advertisement based on a formed key (assuming pointing to a well formed string)
//fields currently hard code, cannot handle more than 256 (arrlength (const above)).  unknown behavior beyond this point.
 struct integratedAdvert lus_getIA(char* key){
	 if(DISABLE)
	 {
		 return 0;
	 }

	char value[ARRLENGTH];
	memset (value, 0, sizeof(value));

	//init advert
	struct integratedAdvert advert;
	initIntegratedAdvert(&advert);


	redisReply* reply = redisCommand(context, "hgetall %s", key, strlen(key) );
	if (reply->type == REDIS_REPLY_ARRAY) {
		//only of elements of type path attribute being returned now, just for peering experiments
		struct field aField[ARRLENGTH]; //HACK IN FOR PEERING EXPERIMENT, MUST GENERALIZE LATER
		int currentField = 0;
		for (int i = 0; i < reply->elements; i++) {
			printf("[debug] element received: %s\n", reply->element[i]->str);
			if(i % 2 == 0)
			{
//				advert.pathAttributes->fields = malloc(sizeof(struct protoFields));
				aField[currentField].fieldName = malloc(strlen(reply->element[i]->str));
				strcpy(aField[currentField].fieldName, reply->element[i]->str);
				aField[currentField].fieldNameSize = strlen(reply->element[i]->str);
			}
			else
			{
				aField[currentField].aFieldValue = malloc(strlen(reply->element[i]->str));
				strcpy(aField[currentField].aFieldValue, reply->element[i]->str);
				aField[currentField].fieldValSize = strlen(reply->element[i]->str);
				currentField++;
			}
		}

		advert.pathAttributes->fields->fields = aField;
		advert.pathAttributes->fields->numFields = currentField;
	}
	return advert;
}

//function that just creates pointers to the various fields in an integrated advertisement.
void lus_initIntegratedAdvert(struct integratedAdvert* advert)
{
	advert->pathAttributes = malloc(sizeof(struct pathDescriptors));
	advert->pathAttributes->next = NULL;
	advert->pathAttributes->fields = malloc(sizeof(struct protoFields));
	advert->pathAttributes->fields->numFields = 0;
	advert->pathAttributes->fields->protoID = WISER;
}

redisContext* lus_getContext()
{
	return context;
}

