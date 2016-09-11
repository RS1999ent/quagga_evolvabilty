/* Creates a empty integrated advertisement and returns a reference to its
   serialized form.

   Arguments:
      size: ptr to an int that will be mutated with the length of the serialized
      instance.

   Returns: A ptr to the serialized empty protobuf.
*/
extern "C" char* CreateEmptyIntegratedAdvertisement(int *size);

/* Adds a wiser link cost to the integrated advertisement. If the advertisement
   doesn't exist (i.e. has a size of 0) then this function creates it and puts
   the cost for wiser in it.

   Arguments;
      serialized_advert: ptr to the serialized advertisement. Ownership is
      retained by caller.
      advert_size: the size of the serialized_advert. Expected >=0
      additive_path_cost: the cost to add to the advertisement.
      additive_path_cost: the cost to add on to the 'PathCost' attribute of wiser.
      modified_advert_size: The size of the advertisement after the cost has
      been added.

   Returns: A ptr to a new serialized advertisement. Ownership is passed to caller.

*/
extern "C" char* SetWiserControlInfo(char *serialized_advert,
                                     int advert_size,  int additive_path_cost,
                                     int *modified_advert_size);


/* Gets the wiser path cost associated with this serialized advert.

   @Note:
   Assumption: Wiser nodes are making the assumption that there is always a path
   cost (which there will be because they update the link cost on the incoming
   link). Note for if this ever changes.

   Arguments:
      serialized_advert: the serialized advert that is a serialized version of a
      protobuf.
      advert_size: the size of this serialized protobuf.

   Returns: the path coste associated with this advertisement.
 */
extern "C" int GetWiserPathCost(char* serialized_advert, int advert_size);

/* Returns the last wiser node that changed the path cost.

   Arguments:
      serialized_advert: the IA proto in serialized form

   Returns: -1 if there was no previous wiser node that touched the node.
   Otherwise it will be the as that touched it last

 */
extern "C" int GetLastWiserNode(char* serialized_advert, int advert_size);

/* Sets a new LastWiserNode key value.  If it doesn't exist, it creates one and sets it

   Arguments:
      serialized_advert: The IA proto in serialized form that we are updateing.
      advet_size: the size of this advert in bytes
      new_last_node: the new last wiser node we are setting
      return_advert_size: The size of the returned new serialized advert

   Returns: A serialized advert with the new information set
 */
extern "C" char* SetLastWiserNode(char* serialized_advert, int advert_size, int new_last_node, int *return_advert_size);