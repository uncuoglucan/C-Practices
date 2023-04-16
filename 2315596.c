#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct AOI
{
    char name;
	int topLeftX;
	int width;
    int topLeftY;
    int height;
    int fixationCount;
    int dwellTime;
};
struct data
{
    int index;
    int x;
    int y;
    int duration;
};

struct AOI *AOIarray(char path[100], int *AOICount)
{

	int counter = 0;
	struct AOI *node_AOI = NULL;
    //AOIs path taken until the ...directory\ and this part adds the file name (aois.txt) to the path
    strcat(path, "aois.txt");
	FILE* AOI_txt = fopen(path, "r");

	node_AOI = (struct AOI*)malloc(sizeof(struct AOI));

	while (!feof(AOI_txt))
	{

		fscanf(AOI_txt,"%c %d %d %d %d %d %d", &node_AOI[counter].name, &node_AOI[counter].topLeftX, &node_AOI[counter].width, &node_AOI[counter].topLeftY, &node_AOI[counter].height);
        node_AOI[counter].dwellTime =0;
        node_AOI[counter].fixationCount =0;

        counter++;
        node_AOI = (struct AOI*)realloc(node_AOI,(counter+1)*sizeof(struct AOI));
	}
	fclose(AOI_txt);

	*AOICount = counter;

	return node_AOI;
}

struct data *data_file_read(char path[100], int PCount, int *DATACount)
{
	int counter = 0;
	struct data *node_data = NULL;
    char index[5], x, y, duration[8];
    int base = 0, copy, divider = 10;
    copy = PCount;

    while(copy != 0)
    {
        copy /= divider;
        base += 1;
        divider *= 10;
    }

    node_data = (struct data*)malloc(sizeof(struct data));

	int file_number = 1;
	char *in;
	char txt_extension[] = ".txt";
    char *temp = (char*)malloc(sizeof(char));
    in = (char*)malloc(base * sizeof(char));

	for(file_number = 1; file_number < PCount + 1; file_number++)
    {
        strcpy(temp, path);
        //data path taken until the ...directory\ and this part adds the file name (file.txt) to the path
        itoa(file_number, in, 10);
        strcat(in, txt_extension);
        strcat(temp, in);

        FILE* data_txt = fopen(temp, "r");
        //pass the names of the column in data files
        fseek(data_txt, 18, SEEK_SET);

        while (!feof(data_txt))
        {
            fscanf(data_txt,"%d %d %d %d", &node_data[counter].index, &node_data[counter].x, &node_data[counter].y, &node_data[counter].duration);

            counter++;
            node_data = (struct data*)realloc(node_data,(counter+1)*sizeof(struct data));
        }

        *DATACount = counter;
        fclose(data_txt);
    }


	return node_data;
}
//Compute Fixation count it check wheter (x,y) cordinate between (topLeftX,topLeftY)-(topLeftX+width,topLeftY+height)
struct AOI* computeFixationCount(struct AOI* ACal, struct data* DCal, int AOI_Count, int Data_Count)
{
    int i, j;
    for(i = 0; i < AOI_Count; i++)
        for(j = 0; j < Data_Count; j++)
        {
            if((ACal[i].topLeftX <= DCal[j].x && DCal[j].x <= (ACal[i].topLeftX + ACal[i].width)) && (ACal[i].topLeftY <=  DCal[j].y && DCal[j].y <=  (ACal[i].topLeftY + ACal[i].height)))
                {
                    ACal[i].fixationCount += 1;
                }
        }

    return ACal;
}
//Compute dwell time it check wheter (x,y) cordinate between (topLeftX,topLeftY)-(topLeftX+width,topLeftY+height)
struct AOI* computeDwellTime(struct AOI* ACal, struct data* DCal, int AOI_Count, int Data_Count)
{
    int i, j;
    for(i = 0; i < AOI_Count; i++)
        for(j = 0; j < Data_Count; j++)
        {
            if((ACal[i].topLeftX<= DCal[j].x && DCal[j].x <= (ACal[i].topLeftX + ACal[i].width)) && (ACal[i].topLeftY<= DCal[j].y && DCal[j].y <= (ACal[i].topLeftY + ACal[i].height)))
                {
                    ACal[i].dwellTime += DCal[j].duration;
                }
        }
    return ACal;
}

void swap(struct AOI* first, struct AOI *max)
{
    struct AOI* tmp  = (struct AOI*)malloc(sizeof(struct AOI));

    tmp->name = first->name;
	tmp->topLeftX = first->topLeftX;
	tmp->width = first->width;
    tmp->topLeftY = first->topLeftY;
    tmp->height = first->height;
    tmp->fixationCount = first->fixationCount;
    tmp->dwellTime = first->dwellTime;

    first->name = max->name;
	first->topLeftX = max->topLeftX;
	first->width = max->width;
    first->topLeftY = max->topLeftY;
    first->height = max->height;
    first->fixationCount = max->fixationCount;
    first->dwellTime = max->dwellTime;

    max->name = tmp->name;
	max->topLeftX = tmp->topLeftX;
	max->width = tmp->width;
    max->topLeftY = tmp->topLeftY;
    max->height = tmp->height;
    max->fixationCount = tmp->fixationCount;
    max->dwellTime = tmp->dwellTime;
}

void Heapify(struct AOI* Heapify_AOI, int size, int counter, int sortingCriteria)
{
    int max = counter;
    int LC  = (2*counter) + 1;
    int RC = (2*counter) + 2;

    if(sortingCriteria == 1)
    {
        if (LC < size && Heapify_AOI[LC].fixationCount > Heapify_AOI[max].fixationCount)
            max = LC;

        if (RC < size && Heapify_AOI[RC].fixationCount > Heapify_AOI[max].fixationCount)
            max = RC;

        if (max != counter)
        {
            swap(&Heapify_AOI[counter], &Heapify_AOI[max]);
            Heapify(Heapify_AOI, size, max, sortingCriteria);
        }
    }


    else if(sortingCriteria == 2)
    {
        if (LC < size && Heapify_AOI[LC].dwellTime > Heapify_AOI[max].dwellTime)
            max = LC;

        if (RC < size && Heapify_AOI[RC].dwellTime > Heapify_AOI[max].dwellTime)
            max = RC;

        if (max != counter)
        {
            swap(&Heapify_AOI[counter], &Heapify_AOI[max]);
            Heapify(Heapify_AOI, size, max, sortingCriteria);
        }
    }
}

void buildHeap(int size, struct AOI* AOI_Build_Heap, int sortingCriteria)
{
    int counter;
    for (counter = (size/2)-1; 0 <= counter; counter--)
        Heapify(AOI_Build_Heap, size, counter, sortingCriteria);
}

void heapSort(struct AOI* Heap_AOI, int size, int sortingCriteria)
{

    int counter;

    buildHeap(size, Heap_AOI, sortingCriteria);

    for (counter=(size-1); counter >= 0; counter--)
    {
        swap(&Heap_AOI[0], &Heap_AOI[counter]);
        Heapify(Heap_AOI, counter, 0, sortingCriteria);
    }
}


void printAOIs(struct AOI* AOI_sort, int cAOI)
{
    printf("\nAOI\tFixation-count\tDwell-time\n");
    for(int i = 0; i < cAOI; i++)
    {
        printf("%c\t%d\t\t%d ms\n", AOI_sort[i].name,AOI_sort[i].fixationCount, AOI_sort[i].dwellTime);
    }
}

int main(int argc,char* argv[])
{
    int sortingCriteria, ParticipantCount;
    char AOIsFile[100], DataFolder[100], eyeAnalyser[100];

    int counter_AOI, counter_Data;

    struct AOI  *AOI_datas = NULL;
    struct data *DATA_datas = NULL;

    struct AOI  *Calculation_datas = NULL;


    if(argc==1)
        printf("\nOnly Prgpram named entered. Please enter more command line argument.");
    if(argc>=2)
    {
        printf("\nNumber Of Arguments Passed: %d",argc);
        printf("\n|--The Command Line Arguments--|");

        strcpy(eyeAnalyser, argv[0]);
        sortingCriteria = atoi(argv[1]);
        strcpy(AOIsFile, argv[2]);
        strcpy(DataFolder, argv[3]);
        ParticipantCount = atoi(argv[4]);
        //prints the taken command line arguments.
        printf("\neyeAnalyser is %s", eyeAnalyser);
        printf("\nsortingCriteria is %d", sortingCriteria);
        printf("\nAOIsFile is %s", AOIsFile);
        printf("\nDataFolder is %s", DataFolder);
        printf("\nParticipantCount is %d\n", ParticipantCount);
    }

	AOI_datas = AOIarray(AOIsFile, &counter_AOI);
	DATA_datas = data_file_read(DataFolder, ParticipantCount, &counter_Data);

    Calculation_datas = computeFixationCount(AOI_datas, DATA_datas, counter_AOI, counter_Data);
    Calculation_datas = computeDwellTime(AOI_datas, DATA_datas, counter_AOI, counter_Data);


    printf("\nUnsorted array:\n");
    printAOIs(Calculation_datas, counter_AOI);

    heapSort(Calculation_datas, counter_AOI, sortingCriteria);

    printf("\nSorted array:\n");
    printAOIs(Calculation_datas, counter_AOI);


    return 0;
}
