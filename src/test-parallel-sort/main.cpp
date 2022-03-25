#include <aRibeiroCore/aRibeiroCore.h>
#include <aRibeiroPlatform/aRibeiroPlatform.h>
using namespace aRibeiro;
#include <algorithm>
#include "additional_algorithms.h"

void signal_handler(int signal) {
    printf("   ****************** signal_handler **********************\n");

    PlatformThread::getMainThread()->interrupt();
}

template<typename T>
void check_sorting(const std::vector<T> &array) {
    printf("    check_sorting: ");
    for (int i = 0; i < array.size() - 1; i++) {
        if (array[i] > array[i + 1]) {
            printf("ERROR\n\n");
            return;
        }
    }
    printf("OK\n\n");
}

int main(int argc, char* argv[]){

    setGlobalThreadPriority(GlobalThreadPriority_Realtime);

    PlatformSignal::Set( signal_handler );
    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));
    // initialize self referencing of the main thread.
    PlatformThread::getMainThread();

    printf("TEST Parallel Sort Algorithms\n"
        "\n"
        "For this is how God loved the world :\n"
        "he gave his only Son, so that everyone\n"
        "who believes in him may not perish\n"
        "but may have eternal life.\n"
        "\n"
        "John 3:16\n\n");

    int NUMBERS_TO_SORT = 500000;
    //int NUMBERS_TO_SORT = 32;
    PlatformTime timer;
    ThreadPool threadPool;
    Sorting::DynamicSort dynamicSort(&threadPool);

    {
        printf("\n"
            "Testing int32_t sorting\n"
            "-----------------------\n"
            "\n");
        std::vector<int32_t> numbers_int32(NUMBERS_TO_SORT);
        std::vector<int32_t> random_int32(NUMBERS_TO_SORT);
        std::vector<int32_t> tmp_buff_int32(NUMBERS_TO_SORT + 2);
        for (int i = 0; i < NUMBERS_TO_SORT; i++)
            random_int32[i] = Random::getRange(INT32_MIN, INT32_MAX);
        random_int32[0] = INT32_MAX;
        random_int32[1] = INT32_MIN;

        float std_time = 0;
        numbers_int32 = random_int32;
        printf("[std::sort]\n");
        timer.update();
        std::sort(numbers_int32.begin(), numbers_int32.end());
        timer.update();
        std_time = timer.deltaTime;
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time/timer.deltaTime);
        check_sorting(numbers_int32);

        numbers_int32 = random_int32;
        printf("[radix_counting_sort_signed]\n");
        timer.update();
        Sorting::radix_counting_sort_signed(&numbers_int32[0], NUMBERS_TO_SORT, &tmp_buff_int32[0]);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_int32);

        numbers_int32 = random_int32;
        printf("[radix_bucket_sort_signed]\n");
        timer.update();
        Sorting::radix_bucket_sort_signed(&numbers_int32[0], NUMBERS_TO_SORT);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_int32);

        //
        // hybrid_bucket
        //
        printf("\n**** hybrid_bucket ****\n\n");

        numbers_int32 = random_int32;
        printf("[hybrid_bucket_std_signed_OpenMP]\n");
        timer.update();
        Sorting::hybrid_bucket_std_signed_OpenMP(&numbers_int32[0], NUMBERS_TO_SORT);
        timer.update();
        std_time = timer.deltaTime;
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_int32);

        numbers_int32 = random_int32;
        printf("[hybrid_bucket_radix_counting_signed_OpenMP]\n");
        timer.update();
        Sorting::hybrid_bucket_radix_counting_signed_OpenMP(&numbers_int32[0], NUMBERS_TO_SORT, &tmp_buff_int32[0]);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_int32);

        numbers_int32 = random_int32;
        printf("[dynamicSort.sort_int32_t DynamicSortGather_bucket DynamicSortAlgorithm_radix_counting]\n");
        timer.update();
        dynamicSort.sort_int32_t(&numbers_int32[0], NUMBERS_TO_SORT, Sorting::DynamicSortGather_bucket, Sorting::DynamicSortAlgorithm_radix_counting);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_int32);

        //
        // hybrid_counting
        //
        printf("\n**** hybrid_counting ****\n\n");

        numbers_int32 = random_int32;
        printf("[hybrid_counting_std_signed_OpenMP]\n");
        timer.update();
        Sorting::hybrid_counting_std_signed_OpenMP(&numbers_int32[0], NUMBERS_TO_SORT);
        timer.update();
        std_time = timer.deltaTime;
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_int32);

        numbers_int32 = random_int32;
        printf("[hybrid_counting_radix_counting_signed_OpenMP]\n");
        timer.update();
        Sorting::hybrid_counting_radix_counting_signed_OpenMP(&numbers_int32[0], NUMBERS_TO_SORT, &tmp_buff_int32[0]);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_int32);

        numbers_int32 = random_int32;
        printf("[dynamicSort.sort_int32_t DynamicSortGather_counting DynamicSortAlgorithm_radix_counting]\n");
        timer.update();
        dynamicSort.sort_int32_t(&numbers_int32[0], NUMBERS_TO_SORT, Sorting::DynamicSortGather_counting, Sorting::DynamicSortAlgorithm_radix_counting);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_int32);

        //
        // hybrid_merge
        //
        printf("\n**** hybrid_merge ****\n\n");

        numbers_int32 = random_int32;
        printf("[hybrid_merge_std_signed_OpenMP]\n");
        timer.update();
        Sorting::hybrid_merge_std_signed_OpenMP(&numbers_int32[0], NUMBERS_TO_SORT);
        timer.update();
        std_time = timer.deltaTime;
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_int32);

        numbers_int32 = random_int32;
        printf("[hybrid_merge_radix_counting_signed_OpenMP]\n");
        timer.update();
        Sorting::hybrid_merge_radix_counting_signed_OpenMP(&numbers_int32[0], NUMBERS_TO_SORT, &tmp_buff_int32[0]);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_int32);

        numbers_int32 = random_int32;
        printf("[dynamicSort.sort_int32_t DynamicSortGather_merge DynamicSortAlgorithm_radix_counting]\n");
        timer.update();
        dynamicSort.sort_int32_t(&numbers_int32[0], NUMBERS_TO_SORT, Sorting::DynamicSortGather_merge, Sorting::DynamicSortAlgorithm_radix_counting);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_int32);

    }

    {
        printf("\n"
            "Testing uint32_t sorting\n"
            "------------------------\n"
            "\n");
        std::vector<uint32_t> numbers_uint32(NUMBERS_TO_SORT);
        std::vector<uint32_t> random_uint32(NUMBERS_TO_SORT);
        std::vector<uint32_t> tmp_buff_uint32(NUMBERS_TO_SORT + 2);//cormen implementation uses 2 positions more in the tmp buffer
        for (int i = 0; i < NUMBERS_TO_SORT; i++)
            random_uint32[i] = (uint32_t)(Random::getDouble() * (double)UINT32_MAX + 0.5);
        random_uint32[0] = UINT32_MAX;
        random_uint32[1] = 0;

        float std_time = 0;
        numbers_uint32 = random_uint32;
        printf("[std::sort]\n");
        timer.update();
        std::sort(numbers_uint32.begin(), numbers_uint32.end());
        timer.update();
        std_time = timer.deltaTime;
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_uint32);

        numbers_uint32 = random_uint32;
        printf("[Cormen_Recursive_MergeSort]\n");
        timer.update();
        Cormen_Recursive_MergeSort(&numbers_uint32[0], NUMBERS_TO_SORT, &tmp_buff_uint32[0]);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_uint32);

        numbers_uint32 = random_uint32;
        printf("[NonRecursive_MergeSort]\n");
        timer.update();
        NonRecursive_MergeSort(&numbers_uint32[0], NUMBERS_TO_SORT, &tmp_buff_uint32[0]);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_uint32);

        numbers_uint32 = random_uint32;
        printf("[NonRecursive_MergeSort2]\n");
        timer.update();
        NonRecursive_MergeSort2(&numbers_uint32[0], NUMBERS_TO_SORT, &tmp_buff_uint32[0]);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_uint32);

        numbers_uint32 = random_uint32;
        printf("[radix_counting_sort_unsigned]\n");
        timer.update();
        Sorting::radix_counting_sort_unsigned(&numbers_uint32[0], NUMBERS_TO_SORT, &tmp_buff_uint32[0]);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_uint32);

        numbers_uint32 = random_uint32;
        printf("[radix_bucket_sort_unsigned]\n");
        timer.update();
        Sorting::radix_bucket_sort_unsigned(&numbers_uint32[0], NUMBERS_TO_SORT);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_uint32);

        //
        // hybrid_bucket
        //
        printf("\n**** hybrid_bucket ****\n\n");

        numbers_uint32 = random_uint32;
        printf("[hybrid_bucket_std_unsigned_OpenMP]\n");
        timer.update();
        Sorting::hybrid_bucket_std_unsigned_OpenMP(&numbers_uint32[0], NUMBERS_TO_SORT);
        timer.update();
        std_time = timer.deltaTime;
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_uint32);

        numbers_uint32 = random_uint32;
        printf("[hybrid_bucket_radix_counting_unsigned_OpenMP]\n");
        timer.update();
        Sorting::hybrid_bucket_radix_counting_unsigned_OpenMP(&numbers_uint32[0], NUMBERS_TO_SORT, &tmp_buff_uint32[0]);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_uint32);

        numbers_uint32 = random_uint32;
        printf("[dynamicSort.sort_uint32_t DynamicSortGather_bucket DynamicSortAlgorithm_radix_counting]\n");
        timer.update();
        dynamicSort.sort_uint32_t(&numbers_uint32[0], NUMBERS_TO_SORT, Sorting::DynamicSortGather_bucket, Sorting::DynamicSortAlgorithm_radix_counting);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_uint32);

        //
        // hybrid_counting
        //
        printf("\n**** hybrid_counting ****\n\n");

        numbers_uint32 = random_uint32;
        printf("[hybrid_counting_std_unsigned_OpenMP]\n");
        timer.update();
        Sorting::hybrid_counting_std_unsigned_OpenMP(&numbers_uint32[0], NUMBERS_TO_SORT);
        timer.update();
        std_time = timer.deltaTime;
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_uint32);

        numbers_uint32 = random_uint32;
        printf("[hybrid_counting_radix_counting_unsigned_OpenMP]\n");
        timer.update();
        Sorting::hybrid_counting_radix_counting_unsigned_OpenMP(&numbers_uint32[0], NUMBERS_TO_SORT, &tmp_buff_uint32[0]);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_uint32);

        numbers_uint32 = random_uint32;
        printf("[dynamicSort.sort_uint32_t DynamicSortGather_counting DynamicSortAlgorithm_radix_counting]\n");
        timer.update();
        dynamicSort.sort_uint32_t(&numbers_uint32[0], NUMBERS_TO_SORT, Sorting::DynamicSortGather_counting, Sorting::DynamicSortAlgorithm_radix_counting);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_uint32);

        //
        // hybrid_merge
        //
        printf("\n**** hybrid_merge ****\n\n");

        numbers_uint32 = random_uint32;
        printf("[hybrid_merge_std_unsigned_OpenMP]\n");
        timer.update();
        Sorting::hybrid_merge_std_unsigned_OpenMP(&numbers_uint32[0], NUMBERS_TO_SORT);
        timer.update();
        std_time = timer.deltaTime;
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_uint32);

        numbers_uint32 = random_uint32;
        printf("[hybrid_merge_radix_counting_unsigned_OpenMP]\n");
        timer.update();
        Sorting::hybrid_merge_radix_counting_unsigned_OpenMP(&numbers_uint32[0], NUMBERS_TO_SORT, &tmp_buff_uint32[0]);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_uint32);

        numbers_uint32 = random_uint32;
        printf("[dynamicSort.sort_uint32_t DynamicSortGather_merge DynamicSortAlgorithm_radix_counting]\n");
        timer.update();
        dynamicSort.sort_uint32_t(&numbers_uint32[0], NUMBERS_TO_SORT, Sorting::DynamicSortGather_merge, Sorting::DynamicSortAlgorithm_radix_counting);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_uint32);

    }

    {
        printf("\n"
            "Testing float sorting as uint32_t\n"
            "------------------------\n"
            "\n");

        std::vector<float> numbers_float(NUMBERS_TO_SORT);
        std::vector<float> random_float(NUMBERS_TO_SORT);

        std::vector<uint32_t> numbers_floati(NUMBERS_TO_SORT);
        std::vector<uint32_t> random_floati(NUMBERS_TO_SORT);
        std::vector<uint32_t> tmp_buff_float(NUMBERS_TO_SORT + 2);
        for (int i = 0; i < NUMBERS_TO_SORT; i++)
            random_float[i] = (Random::getFloat() - 0.5f) * 99999.0f;
        random_float[0] = 99999.0f;
        random_float[1] = -99999.0f;

        for (int i = 0; i < NUMBERS_TO_SORT; i++) {
            random_floati[i] = Sorting::sort_float_to_uint32(random_float[i]);
        }

        float std_time = 0;
        numbers_float = random_float;
        printf("[std::sort]\n");
        timer.update();
        std::sort(numbers_float.begin(), numbers_float.end());
        timer.update();
        std_time = timer.deltaTime;
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_float);

        numbers_floati = random_floati;
        printf("[radix_counting_sort_unsigned]\n");
        timer.update();
        Sorting::radix_counting_sort_unsigned(&numbers_floati[0], NUMBERS_TO_SORT, &tmp_buff_float[0]);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        for (int i = 0; i < NUMBERS_TO_SORT; i++) {
            random_float[i] = Sorting::sort_uint32_to_float(numbers_floati[i]);
        }
        check_sorting(random_float);

        numbers_floati = random_floati;
        printf("[radix_bucket_sort_unsigned]\n");
        timer.update();
        Sorting::radix_bucket_sort_unsigned(&numbers_floati[0], NUMBERS_TO_SORT);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        for (int i = 0; i < NUMBERS_TO_SORT; i++) {
            random_float[i] = Sorting::sort_uint32_to_float(numbers_floati[i]);
        }
        check_sorting(random_float);
    }


    {
        printf("\n"
            "Testing float sorting as int32_t\n"
            "------------------------\n"
            "\n");

        std::vector<float> numbers_float(NUMBERS_TO_SORT);
        std::vector<float> random_float(NUMBERS_TO_SORT);

        std::vector<int32_t> numbers_floati(NUMBERS_TO_SORT);
        std::vector<int32_t> random_floati(NUMBERS_TO_SORT);
        std::vector<int32_t> tmp_buff_float(NUMBERS_TO_SORT + 2);
        for (int i = 0; i < NUMBERS_TO_SORT; i++)
            random_float[i] = (Random::getFloat() - 0.5f) * 99999.0f;
        random_float[0] = 99999.0f;
        random_float[1] = -99999.0f;

        for (int i = 0; i < NUMBERS_TO_SORT; i++) {
            random_floati[i] = Sorting::sort_float_to_int32(random_float[i]);
        }

        float std_time = 0;
        numbers_float = random_float;
        printf("[std::sort]\n");
        timer.update();
        std::sort(numbers_float.begin(), numbers_float.end());
        timer.update();
        std_time = timer.deltaTime;
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        check_sorting(numbers_float);

        numbers_floati = random_floati;
        printf("[radix_counting_sort_signed]\n");
        timer.update();
        Sorting::radix_counting_sort_signed(&numbers_floati[0], NUMBERS_TO_SORT, &tmp_buff_float[0]);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        for (int i = 0; i < NUMBERS_TO_SORT; i++) {
            random_float[i] = Sorting::sort_int32_to_float(numbers_floati[i]);
        }
        check_sorting(random_float);

        numbers_floati = random_floati;
        printf("[radix_bucket_sort_signed]\n");
        timer.update();
        Sorting::radix_bucket_sort_signed(&numbers_floati[0], NUMBERS_TO_SORT);
        timer.update();
        printf("    time -> %f secs\n", timer.deltaTime);
        printf("    SpeedUp -> %f\n", std_time / timer.deltaTime);
        for (int i = 0; i < NUMBERS_TO_SORT; i++) {
            random_float[i] = Sorting::sort_int32_to_float(numbers_floati[i]);
        }
        check_sorting(random_float);
    }


    return 0;
}
