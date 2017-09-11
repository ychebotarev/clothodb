
//------------------------------------------------------------------------------
// This code was generated by a tool.
//
//   Tool : Bond Compiler 0.10.0.0
//   File : record_reflection.h
//
// Changes to this file may cause incorrect behavior and will be lost when
// the code is regenerated.
// <auto-generated />
//------------------------------------------------------------------------------

#pragma once

#include "record_types.h"
#include <bond/core/reflection.h>

namespace example
{
    //
    // Record
    //
    struct Record::Schema
    {
        typedef ::bond::no_base base;

        static const ::bond::Metadata metadata;
        
        private: static const ::bond::Metadata s_name_metadata;
        private: static const ::bond::Metadata s_items_metadata;

        public: struct var
        {
            // name
            typedef struct : ::bond::reflection::FieldTemplate<
                0,
                ::bond::reflection::optional_field_modifier,
                Record,
                std::string,
                &Record::name,
                &s_name_metadata
            > {}  name;
        
            // items
            typedef struct : ::bond::reflection::FieldTemplate<
                1,
                ::bond::reflection::optional_field_modifier,
                Record,
                std::vector<double>,
                &Record::items,
                &s_items_metadata
            > {}  items;
        };

        private: typedef boost::mpl::list<> fields0;
        private: typedef boost::mpl::push_front<fields0, var::items>::type fields1;
        private: typedef boost::mpl::push_front<fields1, var::name>::type fields2;

        public: typedef fields2::type fields;
        
        
        static ::bond::Metadata GetMetadata()
        {
            return ::bond::reflection::MetadataInit("Record", "example.Record",
                ::bond::reflection::Attributes()
            );
        }
    };
    

    
} // namespace example
