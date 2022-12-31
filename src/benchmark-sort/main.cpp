#include <aRibeiroCore/aRibeiroCore.h>
#include <aRibeiroPlatform/aRibeiroPlatform.h>
using namespace aRibeiro;
#include <algorithm>
#include "additional_algorithms.h"

void signal_handler(int signal)
{
    printf("   ****************** signal_handler **********************\n");

    PlatformThread::getMainThread()->interrupt();

    ARIBEIRO_ABORT(true,"forced stopping...");
}

template <typename T>
void check_sorting(const std::vector<T> &array)
{
    printf("    check_sorting: ");
    for (int i = 0; i < array.size() - 1; i++)
    {
        if (array[i] > array[i + 1])
        {
            printf("ERROR\n\n");
            return;
        }
    }
    printf("OK\n\n");
}

#include <locale.h>

int main(int argc, char *argv[])
{

    setlocale(LC_NUMERIC, "pt_BR");

    setGlobalThreadPriority(GlobalThreadPriority_Realtime);

    PlatformSignal::Set(signal_handler);
    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));
    // initialize self referencing of the main thread.
    PlatformThread::getMainThread();

    printf("BENCHMARK Sort Algorithms\n"
           "\n"
           "For this is how God loved the world :\n"
           "he gave his only Son, so that everyone\n"
           "who believes in him may not perish\n"
           "but may have eternal life.\n"
           "\n"
           "John 3:16\n\n");

    // int NUMBERS_TO_SORT = 32;
    PlatformTime timer;
    ThreadPool threadPool;
    Sorting::DynamicSort dynamicSort(&threadPool);

    printf("count;std::sort;mergesort;radixsort;parallel radix\n");

    int test_numbers[] = {100000, 500000, 1000000, 5000000, 10000000, 50000000, 100000000, 500000000};

    for (int i = 0; i < sizeof(test_numbers) / sizeof(int); i++)
    {
        int NUMBERS_TO_SORT = test_numbers[i];
        printf("%i;", NUMBERS_TO_SORT);
        {
            // printf("\n"
            //     "Testing uint32_t sorting\n"
            //     "------------------------\n"
            //     "\n");
            std::vector<uint32_t> numbers_uint32(NUMBERS_TO_SORT);
            std::vector<uint32_t> random_uint32(NUMBERS_TO_SORT);
            std::vector<uint32_t> tmp_buff_uint32(NUMBERS_TO_SORT + 2); // cormen implementation uses 2 positions more in the tmp buffer
            for (int i = 0; i < NUMBERS_TO_SORT; i++)
                random_uint32[i] = (uint32_t)(Random::getDouble() * (double)UINT32_MAX + 0.5);
            // random_uint32[0] = UINT32_MAX;
            // random_uint32[1] = 0;

            float std_time = 0;
            numbers_uint32 = random_uint32;
            // printf("[std::sort]\n");
            timer.update();
            std::sort(numbers_uint32.begin(), numbers_uint32.end());
            timer.update();
            std_time = timer.deltaTime;
            printf("%f;", timer.deltaTime);
            // printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
            // check_sorting(numbers_uint32);

            numbers_uint32 = random_uint32;
            // printf("[Cormen_Recursive_MergeSort]\n");
            timer.update();
            Cormen_Recursive_MergeSort(&numbers_uint32[0], NUMBERS_TO_SORT, &tmp_buff_uint32[0]);
            timer.update();
            printf("%f;", timer.deltaTime);
            // printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
            // check_sorting(numbers_uint32);

            numbers_uint32 = random_uint32;
            // printf("[radix_counting_sort_unsigned]\n");
            timer.update();
            Sorting::radix_counting_sort_unsigned(&numbers_uint32[0], NUMBERS_TO_SORT, &tmp_buff_uint32[0]);
            timer.update();
            printf("%f;", timer.deltaTime);
            // printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
            // check_sorting(numbers_uint32);

            numbers_uint32 = random_uint32;
//            printf("[dynamicSort.sort_uint32_t DynamicSortGather_counting DynamicSortAlgorithm_radix_counting]\n");
            timer.update();
            dynamicSort.sort_uint32_t(&numbers_uint32[0], NUMBERS_TO_SORT, Sorting::DynamicSortGather_counting, Sorting::DynamicSortAlgorithm_radix_counting);
            timer.update();
            printf("%f\n", timer.deltaTime);
            //printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
            //check_sorting(numbers_uint32);
//return 0;
        }
    }

    return 0;
}
