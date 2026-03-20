#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]){

    int rank = 8;
    int size = 10;

    int num_steps = 30;
    int window_size = size/2;
    
    int start_window = 0;
    int end_window = window_size - 1;
    int is_sender;

    for (int i = 0; i < num_steps; i++) {

        is_sender = 0;

        if (rank >= start_window && rank <= end_window) {
            is_sender = 1;
        }
        // window 8,9,0,1
        if (end_window < start_window) {

        	if (rank <= end_window && start_window > rank) {
        		is_sender = 1;
        	}
        	if (rank >= start_window && rank > end_window ) {
        		is_sender = 1;
        	}
        }

        printf("Start %d End %d In %d\n", start_window, end_window, is_sender);

        start_window = (start_window+1) % size;
        end_window = (start_window+window_size-1) % size;
    }
    return 0;
}