int days_from_civil(timedate@ date)
{
	int y = date.year() + 1900;
	const int m = date.mon() + 1;
	const int d = date.mday();

	 y -= m <= 2 ? 1 : 0;
	const int era = (y >= 0 ? y : y-399) / 400;
	const int yoe = y - era * 400;      // [0, 399]
	const int doy = (153*(m + (m > 2 ? -3 : 9)) + 2)/5 + d-1;  // [0, 365]
	const int doe = yoe * 365 + yoe/4 - yoe/100 + doy;         // [0, 146096]
	return era * 146097 + doe - 719468;
}

int days_from_civil(int y, int m, int d)
{
	y -= m <= 2 ? 1 : 0;
	const int era = (y >= 0 ? y : y-399) / 400;
	const int yoe = y - era * 400;      // [0, 399]
	const int doy = (153*(m + (m > 2 ? -3 : 9)) + 2)/5 + d-1;  // [0, 365]
	const int doe = yoe * 365 + yoe/4 - yoe/100 + doy;         // [0, 146096]
	return era * 146097 + doe - 719468;
}

void civil_from_days(int z, int &out y, int &out m, int &out d)
{
	z += 719468;
	const int era = (z >= 0 ? z : z - 146096) / 146097;
	const int doe = z - era * 146097;          // [0, 146096]
	const int yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;  // [0, 399]
	y = yoe + era * 400;
	const int doy = doe - (365*yoe + yoe/4 - yoe/100);                // [0, 365]
	const int mp = (5*doy + 2)/153;                                   // [0, 11]
	d = doy - (153*mp+2)/5 + 1;                             // [1, 31]
	m = mp + (mp < 10 ? 3 : -9);                            // [1, 12]
}