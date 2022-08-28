#include <aRibeiroCore/aRibeiroCore.h>
#include <aRibeiroPlatform/aRibeiroPlatform.h>
using namespace aRibeiro;
#include <algorithm>

void signal_handler(int signal)
{
    printf("   ****************** signal_handler **********************\n");

    PlatformThread::getMainThread()->interrupt();
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

int main(int argc, char *argv[])
{

    setGlobalThreadPriority(GlobalThreadPriority_Realtime);

    PlatformSignal::Set(signal_handler);
    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));
    // initialize self referencing of the main thread.
    PlatformThread::getMainThread();

    printf("TEST Inplace Sort\n"
           "\n"
           "For this is how God loved the world :\n"
           "he gave his only Son, so that everyone\n"
           "who believes in him may not perish\n"
           "but may have eternal life.\n"
           "\n"
           "John 3:16\n\n");

    int NUMBERS_TO_SORT = 500000;

    PlatformTime timer;

    std::vector<int32_t> original_buffer(NUMBERS_TO_SORT);
    for (int i = 0; i < NUMBERS_TO_SORT; i++)
        original_buffer[i] = Random::getRange(INT32_MIN, INT32_MAX);
    check_sorting(original_buffer);

    // sort using aux index
    std::vector<Sorting::IndexInt32> sort_index_buffer(original_buffer.size());
    for (int i = 0; i < (int)sort_index_buffer.size(); i++)
    {
        sort_index_buffer[i].index = i;
        sort_index_buffer[i].toSort = original_buffer[i];
    }
    Sorting::radix_counting_sort_signed_index(&sort_index_buffer[0], sort_index_buffer.size());

    printf("DOUBLE BUFFER METHOD\n");
    std::vector<int32_t> output_buffer(NUMBERS_TO_SORT);
    for (int i = 0; i < (int)sort_index_buffer.size(); i++)
        output_buffer[i] = original_buffer[sort_index_buffer[i].index];
    check_sorting(output_buffer);

    printf("INPLACE REPLACE BUFFER METHOD\n");
    Sorting::inplace_replace(&sort_index_buffer[0], &original_buffer[0], original_buffer.size());
    check_sorting(original_buffer);

    // for (int i = 0; i < (int)original_buffer.size(); i++) {
    //     printf("%i, ", original_buffer[i]);
    // }

    return 0;
}
