#include <aRibeiroCore/aRibeiroCore.h>
#include <aRibeiroPlatform/aRibeiroPlatform.h>
using namespace aRibeiro;
#include <algorithm>

void signal_handler(int signal) {
    printf("   ****************** signal_handler **********************\n");

    PlatformThread::getMainThread()->interrupt();
}

template<typename T>
struct MergeSort_Job{
    T *in;
    T *out;

    int element_count;
    int i;
    int size;

    int normal_sorting_begin;
    int normal_sorting_end;
};

template<typename T>
class MergeSort {

    std::vector<PlatformThread*> threads;
    

    ObjectQueue<MergeSort_Job<T> > queue;
    PlatformSemaphore semaphore;

    void run() {

        while (!PlatformThread::isCurrentThreadInterrupted()){
            
            MergeSort_Job<T> job = queue.dequeue();
            if (queue.isSignaled())
                break;

            if ( job.normal_sorting_begin >=0 ){
                //std::sort(std::begin(v), std::end(v));
                std::sort(&job.in[job.normal_sorting_begin],&job.in[job.normal_sorting_end]);
                semaphore.release();
                continue;
            }

            int write_index = job.i;
            int write_max = job.i + (job.element_count<<1);
            if (write_max > job.size)
                write_max = job.size;

            int a_index = job.i;
            int b_index = job.i+job.element_count;

            int a_max = b_index;
            int b_max = b_index + job.element_count;

            if (a_max > job.size)
                a_max = job.size;
            if (b_max > job.size)
                b_max = job.size;

            while (write_index < write_max && 
                a_index < a_max && 
                b_index < b_max) {

                const T& _a = job.in[a_index];
                const T& _b = job.in[b_index];

                if (_a > _b){
                    job.out[write_index] = _b;
                    b_index++;
                } else {
                    job.out[write_index] = _a;
                    a_index++;
                }
                
                write_index++;
            }

            while (a_index<a_max) {
                job.out[write_index++] = job.in[a_index++];
            }
            while (b_index<b_max) {
                job.out[write_index++] = job.in[b_index++];
            }

            semaphore.release();
        }

    }

public:

    MergeSort():semaphore(0) {

        int num_processors = PlatformThread::QueryNumberOfSystemThreads();

        for(int i=0;i<num_processors;i++){
            threads.push_back(new PlatformThread(this, &MergeSort::run));
        }

        for(int i=0;i<threads.size();i++){
            threads[i]->start();
        }

        
    }

    ~MergeSort() {

        for(int i=0;i<threads.size();i++){
            threads[i]->interrupt();
        }

        for(int i=0;i<threads.size();i++){
            threads[i]->wait();
        }

        for(int i=0;i<threads.size();i++){
            delete threads[i];
        }

        threads.clear();

    }

    int threadCount()const {
        return threads.size();
    }

    void multiThread(T*_array, int size, T*pre_alloc_tmp = NULL) {
        T *_aux;
        if (pre_alloc_tmp == NULL)
            _aux = (T *)malloc_aligned( sizeof(T)*size );
        else
            _aux = pre_alloc_tmp;


        int job_thread_size = size / threadCount();// 1 << 16

        //from 1 to job_thread_size, do normal sorting
        MergeSort_Job<T> job;
        job.in = _array;
        for(int i=0;i<size;i+=job_thread_size){
            job.normal_sorting_begin=i;
            job.normal_sorting_end=i+job_thread_size;
            if (job.normal_sorting_end > size)
                job.normal_sorting_end = size;
            queue.enqueue(job);
        }
        for(int i=0;i<size;i+=job_thread_size){
            if (!semaphore.blockingAcquire())
                return;
        }


        T *in = _array;
        T *out = _aux;

        int element_count = job_thread_size;

        //MergeSort_Job<T> job;
        job.size = size;
        job.normal_sorting_begin = -1;
        job.normal_sorting_end = -1;

        while (element_count < size){

            job.in = in;
            job.out = out;

            job.element_count = element_count;
            
            for(int i=0;i<size;i+=element_count<<1){
                job.i = i;
                queue.enqueue(job);
            }
            
            //block waiting result
            for(int i=0;i<size;i+=element_count<<1){
                if (!semaphore.blockingAcquire())
                    return;
            }

            //swap in/out
            T* aux = in;
            in = out;
            out = aux;

            element_count = element_count << 1;
        }

        if (in != _array)
            memcpy(_array, in, sizeof(T)*size);
        
        if (pre_alloc_tmp == NULL)
            free_aligned(_aux);
    }

    void singleThread(T*_array, int size, T*pre_alloc_tmp = NULL) {
        T *_aux;
        if (pre_alloc_tmp == NULL)
            _aux = (T *)malloc_aligned( sizeof(T)*size );
        else
            _aux = pre_alloc_tmp;

        T *in = _array;
        T *out = _aux;

        int element_count = 1;

        while (element_count < size){

            for(int i=0;i<size;i+=element_count<<1){
                int write_index = i;
                int write_max = i + (element_count<<1);
                if (write_max > size)
                    write_max = size;

                int a_index = i;
                int b_index = i+element_count;

                int a_max = b_index;
                int b_max = b_index + element_count;

                if (a_max > size)
                    a_max = size;
                if (b_max > size)
                    b_max = size;

                while (write_index < write_max && 
                    a_index < a_max && 
                    b_index < b_max) {

                    const T& _a = in[a_index];
                    const T& _b = in[b_index];

                    if (_a > _b){
                        out[write_index] = _b;
                        b_index++;
                    } else {
                        out[write_index] = _a;
                        a_index++;
                    }
                    
                    write_index++;
                }

                while (a_index<a_max) {
                    out[write_index++] = in[a_index++];
                }
                while (b_index<b_max) {
                    out[write_index++] = in[b_index++];
                }
            }

            /*
            printf("-- Merge size: %i\n", element_count);
            printf("in:");
            for(int i=0;i<size;i++){
                if (i % element_count == 0)
                    printf("|");
                else
                    printf(" ");
                printf("%i",in[i]);
            }
            printf("\n");
            printf("out:");
            for(int i=0;i<size;i++){
                if (i % (element_count<<1) == 0)
                    printf("|");
                else
                    printf(" ");
                printf("%i",out[i]);
            }
            printf("\n");
            */

            //swap in/out
            T* aux = in;
            in = out;
            out = aux;

            element_count = element_count << 1;
        }

        if (in != _array)
            memcpy(_array, in, sizeof(T)*size);
        
        if (pre_alloc_tmp == NULL)
            free_aligned(_aux);
    }

};

int main(int argc, char* argv[]){
    PlatformSignal::Set( signal_handler );
    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));
    // initialize self referencing of the main thread.
    PlatformThread::getMainThread();

    printf("TEST Parallel Sort\n"
        "\n"
        "For this is how God loved the world :\n"
        "he gave his only Son, so that everyone\n"
        "who believes in him may not perish\n"
        "but may have eternal life.\n"
        "\n"
        "John 3:16\n\n");

    int count = 99999999;//200000;

    std::vector<int32_t> numbers(count);
    std::vector<int32_t> tmp_buff(count);

    PlatformTime timer;

    MergeSort<int32_t> mergeSort;


    for(int i=0;i<count;i++)
        numbers[i] = Random::getRange(-9999,9999);
    timer.update();
    std::sort(numbers.begin(),numbers.end());
    timer.update();
    printf("std::sort -> %f\n", timer.deltaTime);
    float std_sort = timer.deltaTime;

    for(int i=0;i<count;i++)
        numbers[i] = Random::getRange(-9999,9999);
    timer.update();
    mergeSort.singleThread(&numbers[0], numbers.size(), &tmp_buff[0]);
    timer.update();
    printf("mergeSort.singleThread -> %f\n", timer.deltaTime);
    float merge_sort = timer.deltaTime;
    printf("SpeedUP %f\n", std_sort/merge_sort);


    for(int i=0;i<count;i++)
        numbers[i] = Random::getRange(-9999,9999);
    timer.update();
    mergeSort.multiThread(&numbers[0], numbers.size(),&tmp_buff[0]);
    timer.update();
    printf("mergeSort.multiThread -> %f\n", timer.deltaTime);
    merge_sort = timer.deltaTime;

    printf("SpeedUP %f\n", std_sort/merge_sort);


    for(int i=0;i<numbers.size()-1;i++){
        if (numbers[i] > numbers[i+1]){
            printf("Sorting error...\n");
            printf("%i -> %i %i \n",i, numbers[i], numbers[i+1]);
        }
    }
    
    // for(int i=0;i<numbers.size();i++){
    //     printf("|");
    //     printf("%i",numbers[i]);
    // }
    // printf("\n");

    return 0;
}
