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
