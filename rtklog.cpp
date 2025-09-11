
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <cmath>
#include <ctime>
#include <utility>

static void set_output_file_name(const char* fname, const char* key, char* outfname)
{
	char filename[255] = { 0 }, outfilename[255] = { 0 };
	strcpy(filename, fname);
	char* temp = strrchr(filename, '.');
	if (temp) temp[0] = '\0';
	sprintf(outfname, "%s%s", filename, key);
}

static FILE* set_output_file(const char* fname, const char* key, int is_binary)
{
	char filename[255] = { 0 };
	set_output_file_name(fname, key, filename);
	return fopen(filename, is_binary ? "wb" : "w");
}

bool is_check_base_key(int nlen, int data)
{
	return (nlen == 0 && data == '$') || (nlen == 1 && data == 'B') || (nlen == 2 && data == 'A') || (nlen == 3 && data == 'S') || (nlen == 4 && data == 'E') || (nlen == 5 && data == ',') || nlen > 5;
}
bool is_check_rove_key(int nlen, int data)
{
	return (nlen == 0 && data == '$') || (nlen == 1 && data == 'R') || (nlen == 2 && data == 'O') || (nlen == 3 && data == 'V') || (nlen == 4 && data == 'E') || (nlen == 5 && data == ',') || nlen > 5;
}
bool is_check_rovr_key(int nlen, int data)
{
	return (nlen == 0 && data == '$') || (nlen == 1 && data == 'R') || (nlen == 2 && data == 'O') || (nlen == 3 && data == 'V') || (nlen == 4 && data == 'R') || (nlen == 5 && data == ',') || nlen > 5;
}
bool is_check_brdc_key(int nlen, int data)
{
	return (nlen == 0 && data == '$') || (nlen == 1 && data == 'B') || (nlen == 2 && data == 'R') || (nlen == 3 && data == 'D') || (nlen == 4 && data == 'C') || (nlen == 5 && data == ',') || nlen > 5;
}
bool is_check_geod_key(int nlen, int data)
{
	return (nlen == 0 && data == '$') || (nlen == 1 && data == 'G') || (nlen == 2 && data == 'E') || (nlen == 3 && data == 'O') || (nlen == 4 && data == 'D') || (nlen == 5 && data == ',') || nlen > 5;
}
bool is_check_rtcm_key(int nlen, int data)
{
	return (nlen == 0 && data == '$') || (nlen == 1 && data == 'R') || (nlen == 2 && data == 'T') || (nlen == 3 && data == 'C') || (nlen == 4 && data == 'M') || (nlen == 5 && data == ',') || nlen > 5;
}
bool is_check_gnss_key(int nlen, int data)
{
	return (nlen == 0 && data == '$') || (nlen == 1 && data == 'G') || (nlen == 2 && data == 'N') || (nlen == 3 && data == 'S') || (nlen == 4 && data == 'S') || (nlen == 5 && data == ',') || nlen > 5;
}
bool is_check_proc_key(int nlen, int data)
{
	return (nlen == 0 && data == '$') || (nlen == 1 && data == 'P') || (nlen == 2 && data == 'R') || (nlen == 3 && data == 'O') || (nlen == 4 && data == 'C') || (nlen == 5 && data == ',') || nlen > 5;
}
bool is_check_nmea_key(int nlen, int data)
{
	return (nlen == 0 && data == '$') || (nlen == 1 && data == 'N') || (nlen == 2 && data == 'M') || (nlen == 3 && data == 'E') || (nlen == 4 && data == 'A') || (nlen == 5 && data == ',') || nlen > 5;
}

struct alog_t
{
	int nseg;
	int nlen;
	int nbyte;
	int nloc[4];
	char buff[100];
	unsigned long long time_ms;
	FILE* fLOG;
	alog_t()
	{
		nseg = nlen = nbyte = 0;
		time_ms = 0;
		memset(nloc, 0, sizeof(nloc));
		memset(buff, 0, sizeof(buff));
		fLOG = nullptr;
	}
	~alog_t()
	{
		if (fLOG) fclose(fLOG);
		fLOG = nullptr;
	}
	int open_log(const char* fname)
	{
		if (fLOG) fclose(fLOG);
		fLOG = fopen(fname, "rb");
		return fLOG != NULL;
	}
	int read_log_header()
	{
		int ret = 0;
		int data = 0;
		while (fLOG && !feof(fLOG) && (data = fgetc(fLOG)) != EOF)
		{
			if (is_check_geod_key(nlen, data) ||
				is_check_rtcm_key(nlen, data) ||
				is_check_gnss_key(nlen, data) ||
				is_check_base_key(nlen, data) ||
				is_check_rove_key(nlen, data) ||
				is_check_rovr_key(nlen, data) ||
				is_check_brdc_key(nlen, data) ||
				is_check_proc_key(nlen, data) ||
				is_check_nmea_key(nlen, data))
			{
			}
			else
			{
				nlen = 0;
				continue;
			}
			if (nlen == 0)
			{
				nseg = 0;
				nbyte = 0;
				time_ms = 0;
				memset(nloc, 0, sizeof(nloc));
				memset(buff, 0, sizeof(buff));
			}
			if (nlen >= 100)
			{
				nlen = 0;
				continue;
			}
			buff[nlen++] = data;
			if (data == ',')
			{
				nloc[nseg++] = nlen;
				if (nseg == 3)
				{
					buff[nloc[2]] = '\0';
					nbyte = atoi(buff + nloc[1]);
					//printf("%s\n", buff);
					nlen = 0;
					if (nbyte >= 0)
					{
						time_ms = atoll((char*)(buff + nloc[0]));
					}
					else
					{
						time_ms = 0;
					}
					ret = 1;
					break;
				}
			}
		}
		if (!fLOG || feof(fLOG))
			time_ms = 0;
		return ret;
	}
	int read_log_buffer(char* buffer)
	{
		size_t nread = 0;
		if (time_ms && nbyte >= 0 && fLOG && !feof(fLOG) && (nread = fread(buffer, sizeof(char), nbyte + 2, fLOG)) == (nbyte + 2))
		{

		}
		else
		{
			nread = 0;
		}
		nbyte = 0;
		return (int)nread;
	}
};

int process_rtk_log(const char* fname)
{
	int ret = 0;

	FILE* fROVE = NULL;
	FILE* fBASE = NULL;

	alog_t* rove = new alog_t;

	rove->open_log(fname);

	int numofbase = 0;
	int numofrove = 0;

	while (1)
	{
		rove->read_log_header();
		if (rove->time_ms == 0) break;

		char* buffer = new char[rove->nbyte + 2];
		memset(buffer, 0, sizeof(char) * (rove->nbyte + 2));
		int nread = rove->read_log_buffer(buffer);
		if (nread >= 2)
		{
#if defined(_DEBUG)
			printf("%s\n", rove->buff);
#endif
			if (strstr(rove->buff, "$ROVR"))
			{
				if (!fROVE) fROVE = set_output_file(fname, "-rove.nmea", 1);
				if (fROVE)
				{
					fwrite(buffer, sizeof(char), nread - 2, fROVE);
					fflush(fROVE);
				}
				++numofrove;
			}
			else if (strstr(rove->buff, "$BASE"))
			{
				if (!fBASE) fBASE = set_output_file(fname, "-base.rtcm", 1);
				if (fBASE)
				{
					fwrite(buffer, sizeof(char), nread - 2, fBASE);
					fflush(fBASE);
				}
				/* end to output base rinex */
				++numofbase;
			}
		}
		delete[] buffer;
	}

	if (fROVE) fclose(fROVE);
	if (fBASE) fclose(fBASE);

	printf("number of base message %i\n", numofbase);
	printf("number of rove message %i\n", numofrove);

	delete rove;

	return ret;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
	}
	else 
	{
		clock_t start, end;
		start = clock();

		process_rtk_log(argv[1]);

		end = clock();
		printf("time=%f %s\n", (double)(end - start) / CLOCKS_PER_SEC, argv[1]);

	}


	return 0;
}
