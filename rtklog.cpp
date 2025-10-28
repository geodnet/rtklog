
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <cmath>
#include <ctime>
#include <utility>


#define MAX_RTCM_BUF_LEN 1200

#define RTCM3PREAMB 0xD3        /* rtcm ver.3 frame preamble */

static const unsigned int tbl_CRC24Q[] = {
	0x000000,0x864CFB,0x8AD50D,0x0C99F6,0x93E6E1,0x15AA1A,0x1933EC,0x9F7F17,
	0xA18139,0x27CDC2,0x2B5434,0xAD18CF,0x3267D8,0xB42B23,0xB8B2D5,0x3EFE2E,
	0xC54E89,0x430272,0x4F9B84,0xC9D77F,0x56A868,0xD0E493,0xDC7D65,0x5A319E,
	0x64CFB0,0xE2834B,0xEE1ABD,0x685646,0xF72951,0x7165AA,0x7DFC5C,0xFBB0A7,
	0x0CD1E9,0x8A9D12,0x8604E4,0x00481F,0x9F3708,0x197BF3,0x15E205,0x93AEFE,
	0xAD50D0,0x2B1C2B,0x2785DD,0xA1C926,0x3EB631,0xB8FACA,0xB4633C,0x322FC7,
	0xC99F60,0x4FD39B,0x434A6D,0xC50696,0x5A7981,0xDC357A,0xD0AC8C,0x56E077,
	0x681E59,0xEE52A2,0xE2CB54,0x6487AF,0xFBF8B8,0x7DB443,0x712DB5,0xF7614E,
	0x19A3D2,0x9FEF29,0x9376DF,0x153A24,0x8A4533,0x0C09C8,0x00903E,0x86DCC5,
	0xB822EB,0x3E6E10,0x32F7E6,0xB4BB1D,0x2BC40A,0xAD88F1,0xA11107,0x275DFC,
	0xDCED5B,0x5AA1A0,0x563856,0xD074AD,0x4F0BBA,0xC94741,0xC5DEB7,0x43924C,
	0x7D6C62,0xFB2099,0xF7B96F,0x71F594,0xEE8A83,0x68C678,0x645F8E,0xE21375,
	0x15723B,0x933EC0,0x9FA736,0x19EBCD,0x8694DA,0x00D821,0x0C41D7,0x8A0D2C,
	0xB4F302,0x32BFF9,0x3E260F,0xB86AF4,0x2715E3,0xA15918,0xADC0EE,0x2B8C15,
	0xD03CB2,0x567049,0x5AE9BF,0xDCA544,0x43DA53,0xC596A8,0xC90F5E,0x4F43A5,
	0x71BD8B,0xF7F170,0xFB6886,0x7D247D,0xE25B6A,0x641791,0x688E67,0xEEC29C,
	0x3347A4,0xB50B5F,0xB992A9,0x3FDE52,0xA0A145,0x26EDBE,0x2A7448,0xAC38B3,
	0x92C69D,0x148A66,0x181390,0x9E5F6B,0x01207C,0x876C87,0x8BF571,0x0DB98A,
	0xF6092D,0x7045D6,0x7CDC20,0xFA90DB,0x65EFCC,0xE3A337,0xEF3AC1,0x69763A,
	0x578814,0xD1C4EF,0xDD5D19,0x5B11E2,0xC46EF5,0x42220E,0x4EBBF8,0xC8F703,
	0x3F964D,0xB9DAB6,0xB54340,0x330FBB,0xAC70AC,0x2A3C57,0x26A5A1,0xA0E95A,
	0x9E1774,0x185B8F,0x14C279,0x928E82,0x0DF195,0x8BBD6E,0x872498,0x016863,
	0xFAD8C4,0x7C943F,0x700DC9,0xF64132,0x693E25,0xEF72DE,0xE3EB28,0x65A7D3,
	0x5B59FD,0xDD1506,0xD18CF0,0x57C00B,0xC8BF1C,0x4EF3E7,0x426A11,0xC426EA,
	0x2AE476,0xACA88D,0xA0317B,0x267D80,0xB90297,0x3F4E6C,0x33D79A,0xB59B61,
	0x8B654F,0x0D29B4,0x01B042,0x87FCB9,0x1883AE,0x9ECF55,0x9256A3,0x141A58,
	0xEFAAFF,0x69E604,0x657FF2,0xE33309,0x7C4C1E,0xFA00E5,0xF69913,0x70D5E8,
	0x4E2BC6,0xC8673D,0xC4FECB,0x42B230,0xDDCD27,0x5B81DC,0x57182A,0xD154D1,
	0x26359F,0xA07964,0xACE092,0x2AAC69,0xB5D37E,0x339F85,0x3F0673,0xB94A88,
	0x87B4A6,0x01F85D,0x0D61AB,0x8B2D50,0x145247,0x921EBC,0x9E874A,0x18CBB1,
	0xE37B16,0x6537ED,0x69AE1B,0xEFE2E0,0x709DF7,0xF6D10C,0xFA48FA,0x7C0401,
	0x42FA2F,0xC4B6D4,0xC82F22,0x4E63D9,0xD11CCE,0x575035,0x5BC9C3,0xDD8538
};

extern unsigned int crc24q_(const unsigned char* buff, int len)
{
	unsigned int crc = 0;
	int i;

	for (i = 0; i < len; i++) crc = ((crc << 8) & 0xFFFFFF) ^ tbl_CRC24Q[(crc >> 16) ^ buff[i]];
	return crc;
}

extern unsigned int getbitu_(const unsigned char* buff, int pos, int len)
{
	unsigned int bits = 0;
	int i;
	for (i = pos; i < pos + len; i++) bits = (bits << 1) + ((buff[i / 8] >> (7 - i % 8)) & 1u);
	return bits;
}

static unsigned long long utcms2gpsms(unsigned long long ms) {
	return ms > 0 ? ms - 315964800000ull + 18000ull : 0;
}

struct rtcm_buff_t 
{   
	int staid;          /* station id */
	int type;
	int nbyte;          /* number of bytes in message buffer */
	int len;            /* message length (bytes) */
	int sync;
	int crc;
	unsigned char buff[MAX_RTCM_BUF_LEN]; /* message buffer */
	double tow;
	int numofbyte;
	int numofmsg;
	int numofcrc;
	int numofbyte_invalid;
	std::map<int, int> msg_count;
	std::vector<double> time_offsets;
	rtcm_buff_t()
	{
		staid = type = nbyte = len = sync = crc = 0;
		memset(buff, 0, sizeof(buff));
		tow = 0;
		numofbyte = numofmsg = numofcrc = numofbyte_invalid = 0;
	}
	~rtcm_buff_t()
	{
	}
	int input_rtcm3_type(uint8_t data, unsigned long long time_ms)
	{
		int ret = 0, i = 24, j = 0;
		unsigned long long gps_ms = utcms2gpsms(time_ms);
		type = 0;
		crc = 0;
		staid = 0;

		numofbyte++;
		if (nbyte >= MAX_RTCM_BUF_LEN) nbyte = 0;
		if (nbyte == 0) {
			len = 0;
			if (data != RTCM3PREAMB)
			{
				numofbyte_invalid++;
				return 0;
			}
		}
		buff[nbyte++] = data;

		if (nbyte < 3) return 0;
		len = getbitu_(buff, 14, 10) + 3; /* length without parity */
		if (nbyte < len + 3) return 0;
		nbyte = 0;
		/* check parity */
		if (crc24q_(buff, len) != getbitu_(buff, len * 8, 24)) {
			crc = 1;
			numofcrc++;
			return 0;
		}
		i = 24;
		type = getbitu_(buff, i, 12); i += 12;
		msg_count[type]++;
		/* decode rtcm3 message */
		if ((type == 1074 || type == 1075 || type == 1076 || type == 1077) || /* GPS */
			(type == 1094 || type == 1095 || type == 1096 || type == 1097) || /* GAL */
			(type == 1104 || type == 1105 || type == 1106 || type == 1107) || /* SBS */
			(type == 1114 || type == 1115 || type == 1116 || type == 1117) || /* QZS */
			(type == 1134 || type == 1135 || type == 1136 || type == 1137))  /* IRN */
		{
			/* GPS, GAL, SBS, QZS, IRN */
			staid = getbitu_(buff, i, 12);         i += 12;
			tow = getbitu_(buff, i, 30) * 0.001;   i += 30;
			sync = getbitu_(buff, i, 1);           i += 1;
			if (gps_ms > 0)
			{
				int wk = (int)(gps_ms / 604800000ull);
				double dt = (double)(gps_ms % 604800000ull) * 0.001 - tow;
				time_offsets.push_back(dt);
			}
			ret = sync ? 0 : 1;
		}
		else if (type == 1084 || type == 1085 || type == 1086 || type == 1087)  /* GLO */
		{
			/* GLO */
			staid = getbitu_(buff, i, 12);           i += 12;
			double dow = getbitu_(buff, i, 3);           i += 3;
			double tod = getbitu_(buff, i, 27) * 0.001;   i += 27;
			sync = getbitu_(buff, i, 1);           i += 1;
			double new_tow = dow * 24 * 3600 + tod - 3 * 3600 + 18;
			if (new_tow > 0.0 && fabs(new_tow - tow) > (24 * 1800))
			{
				new_tow -= floor((new_tow - tow) / (24 * 1800)) * (24 * 1800);
			}
			new_tow -= floor(new_tow / 604800) * 604800;
			tow = new_tow;
			if (gps_ms > 0)
			{
				int wk = (int)(gps_ms / 604800000ull);
				double dt = (double)(gps_ms % 604800000ull) * 0.001 - tow;
				time_offsets.push_back(dt);
			}
			ret = sync ? 0 : 1;
		}
		else if (type == 1124 || type == 1125 || type == 1126 || type == 1127)  /* BDS */
		{
			/* BDS */
			staid = getbitu_(buff, i, 12);           i += 12;
			double tow = getbitu_(buff, i, 30) * 0.001;   i += 30;
			sync = getbitu_(buff, i, 1);             i += 1;
			tow += 14.0; /* BDT -> GPST */
			tow -= floor(tow / 604800) * 604800;
			if (gps_ms > 0)
			{
				int wk = (int)(gps_ms / 604800000ull);
				double dt = (double)(gps_ms % 604800000ull) * 0.001 - tow;
				time_offsets.push_back(dt);
			}
			ret = sync ? 0 : 1;
		}
		return ret;
	}
	void print_status()
	{
		printf("rtcm message status:\n");
		printf("  number of bytes input: %d\n", numofbyte);
		printf("  number of valid msg:   %d\n", numofmsg);
		printf("  number of crc error:   %d\n", numofcrc);
		printf("  number of invalid byte:%d\n", numofbyte_invalid);
		printf("  message type count:\n");
		for (auto it = msg_count.begin(); it != msg_count.end(); ++it)
		{
			printf("    type %4d: %d\n", it->first, it->second);
		}
		std::sort(time_offsets.begin(), time_offsets.end());
		if (time_offsets.size() > 0)
		{
			int loc68 = (int)(time_offsets.size() * 0.68);
			int loc95 = (int)(time_offsets.size() * 0.95);
			printf("age [min,68,95,max] s   :%7.3f,%7.3f,%7.3f,%7.3f\n", time_offsets[0], time_offsets[loc68], time_offsets[loc95], time_offsets.back());
		}
	}
};






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

	rtcm_buff_t* rtcm = new rtcm_buff_t;

	std::vector<double> time_offsets;

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
				for (int i = 0; i < nread - 2; i++)
				{
					rtcm->input_rtcm3_type(buffer[i], rove->time_ms);
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

	rtcm->print_status();

	delete rtcm;

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
