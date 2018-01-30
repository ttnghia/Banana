#ifndef HAIR_H
#define HAIR_H
/*
Szymon Rusinkiewicz
Princeton University

Hair.h
Photobooth hairstyle format reading
*/

#include <vector>
#include "Vec.h"
#include "XForm.h"
#include "Color.h"
using namespace trimesh;

class Hair {
public:
	typedef std::vector<point> Strand;
	std::vector<Strand> strands, tangents;
	std::vector<Color> colors;

	void clear() { strands.clear(); tangents.clear(); }
	size_t size() const { return strands.size(); }
	bool read(const char *filename, bool flip_strands = false);
	bool write(const char *filename, bool flip_strands = false);
	void compute_tangents();
	void color(const Color &c)
		{ colors.clear(); colors.resize(strands.size(), c); }
	void apply_xf(const xform &xf);
	void subdiv();

private:
	bool read_asc(const char *filename);
	bool read_bin(const char *filename);
	bool write_asc(const char *filename);
	bool write_bin(const char *filename);
};

#endif
