#include "cfg.h"
#include "../io/memfile.h"
#include "../format.h"
namespace reload::config
{
    cfg::cfg()
    {

    }

    cfg::~cfg()
    {
        clear();
    }

    bool cfg::load (const std::string& p_path)
    {
        reload::io::memfile mfile;
        if (!mfile.load(p_path))
        {
            error.message = "Couldn't open file '" + p_path + "'";
            return false;
        }

        return load(mfile.buffer);
    }

    void cfg::clear()
    {
        for (auto item : properties) delete item;
        for (auto item : nodes) delete item;
    }

    bool cfg::load (const std::vector<uint8_t>& p_buffer)
    {
        clear();
        name = "root";
        enum class part {comment, name, body, property, value, array, array_value};
        part at = part::body;
        std::string text;
        node* current_node = this;
        property* current_prop = nullptr;
        std::deque<node*> stack;
        stack.push_back(current_node);
        size_t line = 1;
        for(auto u : p_buffer)
        {
            char c = static_cast<char>(u);

            if (c=='\r') continue;
            if (c=='\n')
            {
                line++;
                continue;
            }

            if (part::body==at)
            {
                if (c==' ' || c=='\t') continue;
                if (c=='{')
                {
                    at = part::body;
                    if (text.length()<1)
                    {
                        error.message = reload::format("Empty node name at line {i}",line);
                        return false;
                    }
                    auto* newnode = new node(text);
                    current_node->nodes.emplace_back(newnode);
                    stack.push_back(current_node->nodes.back());
                    current_node = newnode;
                    text.clear();
                    continue;
                }
                if (c=='=')
                {
                    at = part::property;
                    if (text.length()<1)
                    {
                        error.message = reload::format("Empty property name at line {i}",line);
                        return false;
                    }
                    current_prop = new property(text);
                    current_node->properties.emplace_back(current_prop);
                    text.clear();
                    continue;
                }
                if (c=='}')
                {
                    at = part::body;
                    stack.pop_back();
                    current_node = stack.back();
                    continue;
                }
            }
            if (part::property==at)
            {
                if (c==' ' || c=='\t') continue;
                if (c=='\"')
                {
                    at = part::value;
                    continue;
                }
            }
            if (part::value==at)
            {
                if (c=='\"')
                {
                    at = part::body;
                    current_prop->value = text;
                    text.clear();
                    continue;
                }
            }
            text += c;
        }
        return true;
    }
}
