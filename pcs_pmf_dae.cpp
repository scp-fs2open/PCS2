#include "pcs_file.h"
#include "DAEHandler.h"

pcs_sobj *process_subobj(daeElement* element);

int PCS_Model::LoadFromDAE(std::string filename, AsyncProgress* progress, bool mirror_x, bool mirror_y, bool mirror_z) {
	DAEHandler dae_handler(filename, this, progress, mirror_x, mirror_y, mirror_z);
	return dae_handler.populate();

}


int PCS_Model::SaveToDAE(std::string filename, AsyncProgress* progress, int helpers, int props_as_helpers) {
	DAESaver dae_handler(filename, this, helpers, props_as_helpers, progress);
	return dae_handler.save();
}


