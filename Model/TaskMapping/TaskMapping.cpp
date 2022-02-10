//
// Created by Jamie Cotter on 26/01/2022.
//

#include "TaskMapping.h"
#include "../../Constants/AlgorithmMode/AlgorithmMode.h"
#include <utility>

TaskMapping::TaskMapping(int totalParentCount, std::shared_ptr<Task> task, mapping_type mappingType)
: totalParentCount(totalParentCount), task(std::move(task)), mappingType(mappingType) {}

TaskMapping::TaskMapping(float absoluteStart, float absoluteFinish, float uploadStart, float uploadFinish,
                         int mappedParentCount, int totalParentCount, std::shared_ptr<Task> task,
                         int computationNodeId, mapping_type mappingType) : processingStart(absoluteStart),
                                                                            processingFinish(absoluteFinish),
                                                                            inputUploadStart(uploadStart),
                                                                            inputUploadFinish(uploadFinish),
                                                                            mappedParentCount(mappedParentCount),
                                                                            totalParentCount(totalParentCount),
                                                                            task(std::move(task)),
                                                                            computationNodeId(computationNodeId),
                                                                            mappingType(mappingType) {}

const std::shared_ptr<Task> &TaskMapping::getTask() const {
    return task;
}

void TaskMapping::setTask(const std::shared_ptr<Task> &task) {
    TaskMapping::task = task;
}

int TaskMapping::getMappedParentCount() const {
    return mappedParentCount;
}

void TaskMapping::setMappedParentCount(int mappedParentCount) {
    TaskMapping::mappedParentCount = mappedParentCount;
}

int TaskMapping::getTotalParentCount() const {
    return totalParentCount;
}

void TaskMapping::setTotalParentCount(int totalParentCount) {
    TaskMapping::totalParentCount = totalParentCount;
}

int TaskMapping::getComputationNodeId() const {
    return computationNodeId;
}

void TaskMapping::setComputationNodeId(int computationNodeId) {
    TaskMapping::computationNodeId = computationNodeId;
}

mapping_type TaskMapping::getMappingType() const {
    return mappingType;
}

float TaskMapping::getSortTime() {
    return (this->mappingType == in_progress) ? this->getFinishValue(): this->processingStart;
}

void TaskMapping::setMappingType(mapping_type mappingType) {
    TaskMapping::mappingType = mappingType;
}

float TaskMapping::getProcessingStart() const {
    return this->processingStart;
}

void TaskMapping::setProcessingStart(float processingStart) {
    TaskMapping::processingStart = processingStart;
}

float TaskMapping::getProcessingFinish() const {
    return processingFinish;
}

void TaskMapping::setProcessingFinish(float processingFinish) {
    TaskMapping::processingFinish = processingFinish;
}

float TaskMapping::getInputUploadStart() const {
    return inputUploadStart;
}

void TaskMapping::setInputUploadStart(float inputUploadStart) {
    TaskMapping::inputUploadStart = inputUploadStart;
}

float TaskMapping::getInputUploadFinish() const {
    return inputUploadFinish;
}

void TaskMapping::setInputUploadFinish(float inputUploadFinish) {
    TaskMapping::inputUploadFinish = inputUploadFinish;
}

float TaskMapping::getOutputUploadStart() const {
    return outputUploadStart;
}

void TaskMapping::setOutputUploadStart(float outputUploadStart) {
    TaskMapping::outputUploadStart = outputUploadStart;
}

float TaskMapping::getOutputUploadFinish() const {
    return outputUploadFinish;
}

void TaskMapping::setOutputUploadFinish(float outputUploadFinish) {
    TaskMapping::outputUploadFinish = outputUploadFinish;
}

float TaskMapping::getFinishValue() {
    if(AlgorithmMode::MODE == REACTIVE_BASIC || AlgorithmMode::MODE == REACTIVE_MOBILE)
        return this->computationNodeId != this->task->getSourceMobileId() ? this->outputUploadFinish : this->processingFinish;
    return this->processingFinish;
}
