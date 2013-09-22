#include <assert.h>

#include "sscLogger.h"

void test8bit(const unsigned char *volume1, const unsigned char *volume2, int len, double *average, double *max, double *ppix)
{
	int i, diffpixels = 0, diffamount = 0, diffmax = 0;

	for (i = 0; i < len; i++)
	{
		const unsigned char v1 = volume1[i];
		const unsigned char v2 = volume2[i];
		const int diff = abs(v1 - v2);

		diffamount += diff;
		if (diff > diffmax) diffmax = diff;
		if (diff > 0) diffpixels++;
	}

	*max = diffmax;
	*ppix = (100.0 * (double)diffpixels) / (double)len;
	*average = (double)diffamount / (double)len;
}

void test16bit(const unsigned short *volume1, const unsigned short *volume2, int len, double *average, double *max, double *ppix)
{
	int i, diffpixels = 0, diffamount = 0, diffmax = 0;

	for (i = 0; i < len; i++)
	{
		const unsigned short v1 = volume1[i];
		const unsigned short v2 = volume2[i];
		const int diff = abs(v1 - v2);

		diffamount += diff;
		if (diff > diffmax) diffmax = diff;
		if (diff > 0) diffpixels++;
	}

	*max = diffmax;
	*ppix = (100.0 * (double)diffpixels) / (double)len;
	*average = (double)diffamount / (double)len;	
}

#define DIE(...) do { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); exit(EXIT_FAILURE); } while (0)

int main (int argc, const char * argv[])
{
	void *volume1;
	void *volume2;
	double average;	// average difference
	double max;
	double ppix;	// percentage of pixels that differ
	int bytes, width, height, depth, len, count;
	FILE *fp;

	if (argc < 7) DIE("Usage: snw2diff <bits> <width> <height> <depth> <file1> <file2>");
	bytes = atoi(argv[1]) / 8;
	width = atoi(argv[2]);
	height = atoi(argv[3]);
	depth = atoi(argv[4]);
	len = width * height * depth;
	volume1 = malloc(len * bytes);
	volume2 = malloc(len * bytes);
	fp = fopen(argv[5], "r");
	if (!fp) DIE("Failed to open %s: %s", argv[5], strerror(errno));
	count = fread(volume1, len, bytes, fp);
	if (count != bytes) DIE("Volume1 %s incorrect size, %d expected, got %d", argv[5], bytes, count);
	fclose(fp);
	fp = fopen(argv[6], "r");
	if (!fp) DIE("Failed to open %s: %s", argv[6], strerror(errno));
	count = fread(volume2, len, bytes, fp);
	if (count != bytes) DIE("Volume2 %s incorrect size, %d expected, got %d", argv[5], bytes, count);
	fclose(fp);

	if (bytes == 1) test8bit(volume1, volume2, len, &average, &max, &ppix);
	else if (bytes == 2) test16bit(volume1, volume2, len, &average, &max, &ppix);
	else DIE("Bad bits");

	printf("Difference: Average=%f Max=%f Percentage=%f\n", average, max, ppix);

	return 0;
}
