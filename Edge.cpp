#include "Edge.h"
Edge::Edge() {

}
int Edge::hasFace(Face *nf) {
	for (auto f : _nbFaces) {
		if (f == nf) {
			return 1;
		}
	}
	return 0;
}