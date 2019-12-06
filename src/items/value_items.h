/**
 * @file        value_items.h
 *
 * @author      Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 * @copyright   Apache License Version 2.0
 *
 *      Copyright 2019 Tobias Anker
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#ifndef VALUE_ITEMS_H
#define VALUE_ITEMS_H

#include <common.h>

//===================================================================
// FunctionItem
//===================================================================
struct FunctionItem
{
    enum FunctionType
    {
        UNDEFINED_FUNCTION = 0,
        GET_FUNCTION = 1,
        SPLIT_FUNCTION = 2,
        CONTAINS_FUNCTION = 3
    };

    FunctionType type = UNDEFINED_FUNCTION;
    std::vector<DataValue> arguments;
};

//===================================================================
// ValueItem
//===================================================================
struct ValueItem
{
    enum ValueType
    {
        UNDEFINED_PAIR_TYPE = 0,
        INPUT_PAIR_TYPE = 1,
        OUTPUT_PAIR_TYPE = 2,
        COMPARE_EQUAL_PAIR_TYPE = 3,
        COMPARE_UNEQUAL_PAIR_TYPE = 4,
    };

    DataItem* item = nullptr;
    ValueType type = INPUT_PAIR_TYPE;
    bool isIdentifier = false;
    std::vector<FunctionItem> functions;

    ValueItem() {}
    ValueItem(const ValueItem &other)
    {
        if(item != nullptr) {
            delete item;
        }
        item = other.item->copy();
        type = other.type;
        isIdentifier = other.isIdentifier;
        functions = other.functions;
    }
    ~ValueItem()
    {
        if(item != nullptr) {
            delete item;
        }
    }
    ValueItem &operator=(const ValueItem &other)
    {
        if(this != &other)
        {
            if(this->item != nullptr) {
                delete this->item;
            }
            this->item = other.item->copy();
            this->type = other.type;
            this->isIdentifier = other.isIdentifier;
            this->functions = other.functions;
        }
        return *this;
    }

};

//===================================================================
// ValueItemMap
//===================================================================
struct ValueItemMap
{
    std::map<std::string, ValueItem> valueMap;

    bool insert(const std::string &key, DataItem* value, bool force=true)
    {
        ValueItem valueItem;
        valueItem.item = value->copy();
        return insert(key, valueItem, force);
    }

    bool insert(const std::string &key, ValueItem &value, bool force=true)
    {
        std::map<std::string, ValueItem>::iterator it;
        it = valueMap.find(key);

        if(it != valueMap.end()
                && force == false)
        {
            return false;
        }

        if(it != valueMap.end()) {
            it->second = value;
        } else {
            valueMap.insert(std::pair<std::string, ValueItem>(key, value));
        }
        return true;
    }

    bool contains(const std::string &key)
    {
        std::map<std::string, ValueItem>::iterator it;
        it = valueMap.find(key);

        if(it != valueMap.end())
        {
            return true;
        }
        return false;
    }

    bool remove(const std::string &key)
    {
        std::map<std::string, ValueItem>::iterator it;
        it = valueMap.find(key);

        if(it != valueMap.end())
        {
            valueMap.erase(it);
            return true;
        }

        return false;
    }

    std::string getValueAsString(const std::string &key)
    {
        std::map<std::string, ValueItem>::iterator it;
        it = valueMap.find(key);
        if(it != valueMap.end()) {
            return it->second.item->toString();
        }

        return "";
    }

    DataItem* get(const std::string &key)
    {
        std::map<std::string, ValueItem>::iterator it;
        it = valueMap.find(key);
        if(it != valueMap.end()) {
            return it->second.item;
        }

        return nullptr;
    }

    uint64_t size()
    {
        return valueMap.size();
    }
};

#endif // VALUE_ITEMS_H