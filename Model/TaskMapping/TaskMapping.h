//
// Created by Jamie Cotter on 26/01/2022.
//

#ifndef UNTITLED2_TASKMAPPING_H
#define UNTITLED2_TASKMAPPING_H

#include <vector>
#include <unordered_map>
#include "memory"
#include "../Task/Task.h"
#include "../../enums/MappingType.h"

class TaskMapping {
private:
    float processingStart       = 0.0f;
    float processingFinish      = 0.0f;
    float inputUploadStart      = 0.0f;
    float inputUploadFinish     = 0.0f;
    float outputUploadStart     = 0.0f;
    float outputUploadFinish    = 0.0f;

    int mappedParentCount  = 0;
    int totalParentCount   = 0;

    std::shared_ptr<Task> task;
    int computationNodeId = 0;

    mapping_type mappingType;

public:

    std::vector<std::shared_ptr<TaskMapping>> parent_mappings;
    // key -1 represents the output upload time back to source for reactive,
    // the rest of the keys are the IDs of parent tasks
    std::unordered_map<int, std::vector<std::shared_ptr<std::pair<float, float>>>> upload_times;

    TaskMapping(int totalParentCount, std::shared_ptr<Task> task, mapping_type mappingType);

    TaskMapping(float absoluteStart, float absoluteFinish, float uploadStart, float uploadFinish, int mappedParentCount,
                int totalParentCount, std::shared_ptr<Task> task, int computationNodeId,
                mapping_type mappingType);

    mapping_type getMappingType() const;

    int getComputationNodeId() const;

    void setComputationNodeId(int computationNodeId);

    int getTotalParentCount() const;

    void setTotalParentCount(int totalParentCount);

    int getMappedParentCount() const;

    void setMappedParentCount(int mappedParentCount);

    float getProcessingStart() const;

    void setProcessingStart(float processingStart);

    float getProcessingFinish() const;

    void setProcessingFinish(float processingFinish);

    float getInputUploadStart() const;

    void setInputUploadStart(float inputUploadStart);

    float getInputUploadFinish() const;

    void setInputUploadFinish(float inputUploadFinish);

    float getOutputUploadStart() const;

    void setOutputUploadStart(float outputUploadStart);

    float getOutputUploadFinish() const;

    void setOutputUploadFinish(float outputUploadFinish);

    const std::shared_ptr<Task> &getTask() const;

    void setTask(const std::shared_ptr<Task> &task);

    void setMappingType(mapping_type mappingType);

    float getSortTime();

    float getFinishValue();
};


#endif //UNTITLED2_TASKMAPPING_H
