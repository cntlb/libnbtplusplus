/*
 * libnbt++ - A library for the Minecraft Named Binary Tag format.
 * Copyright (C) 2013, 2015  ljfa-ag
 *
 * This file is part of libnbt++.
 *
 * libnbt++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libnbt++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libnbt++.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "text/json_formatter.h"
#include "nbt_tags.h"
#include "nbt_visitor.h"
#include <cmath>

namespace nbt
{
namespace text
{

namespace //anonymous
{
    class json_fmt_visitor : public const_nbt_visitor
    {
    public:
        json_fmt_visitor(std::ostream& os): os(os) {}

        void visit(const tag_byte& b) override
        { os << static_cast<int>(b.get()) << "b"; } //We don't want to print a character

        void visit(const tag_short& s) override
        { os << s.get() << "s"; }

        void visit(const tag_int& i) override
        { os << i.get(); }

        void visit(const tag_long& l) override
        { os << l.get() << "l"; }

        void visit(const tag_float& f) override
        {
            write_double(f.get());
            os << "f";
        }

        void visit(const tag_double& d) override
        {
            write_double(d.get());
            os << "d";
        }

        void visit(const tag_byte_array& ba) override
        { os << "[" << ba.size() << " bytes]"; }

        void visit(const tag_string& s) override
        { os << '"' << s.get() << '"'; } //TODO: escape special characters

        void visit(const tag_list& l) override
        {
            os << "[\n";
            ++indent_lvl;
            for(unsigned int i = 0; i < l.size(); ++i)
            {
                indent();
                if(l[i])
                    l[i].get().accept(*this);
                else
                    write_null();
                if(i != l.size()-1)
                    os << ",";
                os << "\n";
            }
            --indent_lvl;
            indent();
            os << "]";
        }

        void visit(const tag_compound& c) override
        {
            os << "{\n";
            ++indent_lvl;
            unsigned int i = 0;
            for(const auto& kv: c)
            {
                indent();
                os << kv.first << ": ";
                if(kv.second)
                    kv.second.get().accept(*this);
                else
                    write_null();
                if(i != c.size()-1)
                    os << ",";
                os << "\n";
                ++i;
            }
            --indent_lvl;
            indent();
            os << "}";
        }

        void visit(const tag_int_array& ia) override
        {
            os << "[";
            for(unsigned int i = 0; i < ia.size(); ++i)
            {
                os << ia[i];
                if(i != ia.size()-1)
                    os << ", ";
            }
            os << "]";
        }

    private:
        const std::string indent_str = "  ";

        std::ostream& os;
        int indent_lvl = 0;

        void indent()
        {
            for(int i = 0; i < indent_lvl; ++i)
                os << indent_str;
        }

        void write_double(double d)
        {
            if(std::isfinite(d))
                os << d;
            else if(std::isinf(d))
            {
                if(std::signbit(d))
                    os << "-";
                os << "Infinity";
            }
            else
                os << "NaN";
        }

        void write_null()
        {
            os << "null";
        }
    };
}

void json_formatter::write(std::ostream& os, const tag& t)
{
    json_fmt_visitor v(os);
    t.accept(v);
}

}
}
