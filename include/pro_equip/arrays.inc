#if defined _pro_arrays_included
    #endinput
#endif
#define _pro_arrays_included


/**
 * Removes the first matching item from an array.
 * 
 * @param arr       The array
 * @param size      Number of elements in the array
 * @param value     The value to remove
 * @return          Returns whether an item was removed or not
 */
stock bool remove_item(any[] arr, int &size, any value) {
    bool result = false;
    for(int i=0; i < size; i++) {
        if(!result && arr[i] == value) {
            result = true;
            size--;
        }
        if(result) {
            // the item has been found - shift all elements by copying the following value in the current value
            arr[i] = arr[i+1];
        }
    }
    return result;
}

/**
 * Remove an item from an array by swapping and truncating.  Use this when order does not matter.
 * 
 * The last item in the array is swapped with the item to be removed, and the
 * array size is decreased by one.
 * 
 * Faster than removing an item and shifting all items by one.
 * 
 * @param arr       The array
 * @param size      The number of actual elements in the array
 * @param value     Value to remove
 * @return          Returns whether an item was removed or not
 */
stock bool remove_item_truncate(any[] arr, int &size, any pos) {
    int last = size-1;
    for(int i=0; i < size; i++) {
        if(arr[i] == pos) {
            if(i != last) {
                any tmp = arr[i];
                arr[i] = arr[last];
                arr[last] = tmp;
            }
            size--;
            return true;
        }
    }
    return false;
}

/**
 * Removes all items in an array matching a specified value
 * 
 * This just calls remove_item until all instances of value have been removed
 * 
 * @param arr       The array
 * @param size      The number of actual elements in the array
 * @param value     The value to remove from the array
 * @return          Returns the number of elements removed
 */
stock int remove_item_all(any[] arr, int &size, any value) {
    int i=0;
    while(remove_item(arr, size, value)) {
        i++;
    }
    return i;
}

// Adds an item to a static array
// Returns false if trying to add a value that already exists in the array and unique is true
// Assumes max size of array is at least size+1 (check before using)

/**
 * Append an item to the end of an array.
 * 
 * Assumes max size of array is at least size+1 (check before using)
 * 
 * @param arr        The array (actual size must be larger than than `size`)
 * @param size       The number of actual elements in the array
 * @param value      The value to append
 * @param unique     If unique is true check for an existing element first
 * @return           Returns true if successful
 */
stock bool add_item(any[] arr, int &size, any value, bool unique) {
    if(unique) {
        for(int i=0; i < size; i++) {
            if(arr[i] == value) {
                return false;
            }
        }
    }
    arr[size++] = value;
    return true;
}

/**
 * Insert an item into an array at the specified position, moving all following elements by 1.
 * 
 * Assumes max size of array is at least size+1 (check before using)
 * 
 * @param arr        The array (actual size must be larger than than `size`)
 * @param size       The number of actual elements in the array
 * @param value      The value to insert
 * @param unique     If unique is true check for an existing element first
 * @param at         Where to insert the item
 * @return           Returns true is successful
 */
stock bool add_item_at(any[] arr, int &size, any value, bool unique, int at) {
    if(unique) {
        for(int i=0; i < size; i++) {
            if(arr[i] == value) {
                return false;
            }
        }
    }
    for(int i=size-2; i >= at; i--) {
        arr[i+1] = arr[i];
    }
    arr[at] = value;
    size++;
    return true;
}

/**
 * Find where to insert an item back into an array to retain original ordering.
 * 
 * Given an array of original items and an array of current items (in the same order),
 * find where to insert an item into cur to restore its original relative position.
 * 
 * This is useful when adding/removing menu items (like for weapons) that should reclaim
 * their original position when adding an item back after it was removed.
 * 
 * Assumes two arrays where cur is a subset of defaults and items are in the same order.
 * 
 * @param originals          The array of original items
 * @param original_size     The number of items in originals
 * @param cur               The array of current items
 * @param cur_size          The number of items in cur
 * @param w                 The position of the item in the originals array
 * @return                  Returns the position of cur where the item should be inserted
 */
stock int add_position(const int[] originals, int original_size, const int[] cur, int cur_size, int w) {
    int c, d = 0;
    if(w < 0 || w > original_size) {
        return -1;
    }
    while(c < cur_size && d++ < original_size) {
        if(d > w) {
            return c;
        }
        if(originals[d] == cur[c]) {
            c++;
        }
    }
    return c;
}

