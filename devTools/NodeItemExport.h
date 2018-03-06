#ifndef __NODEITEMEXPORT_H__
#define __NODEITEMEXPORT_H__

#include "NodeItem.h"

class NodeItemExport : public NodeItem {
public:
    NodeItemExport (std::string& fileNameToExport);

    static NodeItemExport* create (
        LayerUserCtrlEditor* pUserCtrlEditor,
        const std::string floder,
        std::string& fileNameToExport);

    virtual bool init (LayerUserCtrlEditor* pUserCtrlEditor, const std::string floder);

    virtual std::string checkParams();
private:
    std::string m_floder;
    std::string& m_fileNameToExport;
};
#endif // !__NODEITEMSCALE_H__
