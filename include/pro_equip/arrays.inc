#if defined _pro_arrays_included
    #endinput
#endif
#define _pro_arrays_included


// removes an item from a static array - assumes an array of unique values
stock bool remove_unqiue_item(int[] arr, int &size, int value) {
    bool result = false;
    for(int i=0; i < size; i++) {
        if(!result && arr[i] == value) {
            result = true;
            size--;
        }
        if(result) {
            arr[i] = arr[i+1];
        }
    }
    return result;
}

// Does not preserver order but should be faster
// Swap the value to last position and truncate by one (no items are technically deleted from memory, just ignored)
stock bool remove_item_truncate(int[] arr, int &size, int value) {
    int last = size-1;
    for(int i=0; i < size; i++) {
        if(arr[i] == value) {
            if(i != last) {
                int tmp = arr[i];
                arr[i] = arr[last];
                arr[last] = tmp;
            }
            size--;
            return true;
        }
    }
    return false;
}

// removes all repeating entries of an array (unique or non-unique)
stock int remove_all_items(int[] arr, int &size, int value) {
    int i=0;
    while(remove_unqiue_item(arr, size, value)) {
        i++;
    }
    return i;
}

// Adds an item to a static array - returns false if trying to add a value that already exists in the array
// Assumes max size of array is at least size+1 (check before using)
stock bool add_item(int[] arr, int &size, int value, bool unique) {
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

// Insert item at index and move everything else over by one (increases size by one).
// Assumes max size of array is at least size+1 (check before using)
stock bool add_item_at(int[] arr, int &size, int value, bool unique, int at) {
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

// assumes two sorted arrays where cur is a subset of defaults.  w is the original position of the value in the defaults array
stock int add_position(const int[] defaults, int defaults_size, const int[] cur, int cur_size, int w) {
    int c, d = 0;
    if(w < 0 || w > defaults_size) {
        // LogToFileEx(EquipLogFile, "add_position: invalid size w=%i", w);
        return -1;
    }
    while(c < cur_size && d++ < defaults_size) {
        if(d > w) {
            // LogToFileEx(EquipLogFile, "add_position: reached w - c=%i d=%i w=%i", c, d, w);
            return c;
        }
        if(defaults[d] == cur[c]) {
            c++;
        }
    }
    // LogToFileEx(EquipLogFile, "add_position: end reached - c=%i d=%i w=%i", c, d, w);
    return c;
}

