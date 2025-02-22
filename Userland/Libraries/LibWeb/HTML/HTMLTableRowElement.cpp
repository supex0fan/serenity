/*
 * Copyright (c) 2020-2022, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/DOM/ElementFactory.h>
#include <LibWeb/DOM/HTMLCollection.h>
#include <LibWeb/HTML/HTMLTableCellElement.h>
#include <LibWeb/HTML/HTMLTableElement.h>
#include <LibWeb/HTML/HTMLTableRowElement.h>
#include <LibWeb/HTML/HTMLTableSectionElement.h>
#include <LibWeb/Namespace.h>

namespace Web::HTML {

HTMLTableRowElement::HTMLTableRowElement(DOM::Document& document, DOM::QualifiedName qualified_name)
    : HTMLElement(document, move(qualified_name))
{
    set_prototype(&Bindings::cached_web_prototype(realm(), "HTMLTableRowElement"));
}

HTMLTableRowElement::~HTMLTableRowElement() = default;

void HTMLTableRowElement::visit_edges(Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_cells);
}

// https://html.spec.whatwg.org/multipage/tables.html#dom-tr-cells
JS::NonnullGCPtr<DOM::HTMLCollection> HTMLTableRowElement::cells() const
{
    // The cells attribute must return an HTMLCollection rooted at this tr element,
    // whose filter matches only td and th elements that are children of the tr element.
    if (!m_cells) {
        m_cells = DOM::HTMLCollection::create(const_cast<HTMLTableRowElement&>(*this), [this](Element const& element) {
            return element.parent() == this
                && is<HTMLTableCellElement>(element);
        });
    }
    return *m_cells;
}

// https://html.spec.whatwg.org/multipage/tables.html#dom-tr-rowindex
int HTMLTableRowElement::row_index() const
{
    // The rowIndex attribute must, if this element has a parent table element,
    // or a parent tbody, thead, or tfoot element and a grandparent table element,
    // return the index of this tr element in that table element's rows collection.
    // If there is no such table element, then the attribute must return −1.
    auto rows_collection = [&]() -> JS::GCPtr<DOM::HTMLCollection> {
        if (!parent())
            return nullptr;
        if (is<HTMLTableElement>(*parent()))
            return const_cast<HTMLTableElement&>(static_cast<HTMLTableElement const&>(*parent())).rows();
        if (is<HTMLTableSectionElement>(*parent()) && parent()->parent() && is<HTMLTableElement>(*parent()->parent()))
            return const_cast<HTMLTableElement&>(static_cast<HTMLTableElement const&>(*parent()->parent())).rows();
        return nullptr;
    }();
    if (!rows_collection)
        return -1;
    auto rows = rows_collection->collect_matching_elements();
    for (size_t i = 0; i < rows.size(); ++i) {
        if (rows[i] == this)
            return i;
    }
    return -1;
}

int HTMLTableRowElement::section_row_index() const
{
    // The sectionRowIndex attribute must, if this element has a parent table, tbody, thead, or tfoot element,
    // return the index of the tr element in the parent element's rows collection
    // (for tables, that's HTMLTableElement's rows collection; for table sections, that's HTMLTableSectionElement's rows collection).
    // If there is no such parent element, then the attribute must return −1.
    auto rows_collection = [&]() -> JS::GCPtr<DOM::HTMLCollection> {
        if (!parent())
            return nullptr;
        if (is<HTMLTableElement>(*parent()))
            return const_cast<HTMLTableElement&>(static_cast<HTMLTableElement const&>(*parent())).rows();
        if (is<HTMLTableSectionElement>(*parent()))
            return static_cast<HTMLTableSectionElement const&>(*parent()).rows();
        return nullptr;
    }();
    if (!rows_collection)
        return -1;
    auto rows = rows_collection->collect_matching_elements();
    for (size_t i = 0; i < rows.size(); ++i) {
        if (rows[i] == this)
            return i;
    }
    return -1;
}

// https://html.spec.whatwg.org/multipage/tables.html#dom-tr-insertcell
WebIDL::ExceptionOr<JS::NonnullGCPtr<HTMLTableCellElement>> HTMLTableRowElement::insert_cell(i32 index)
{
    auto cells_collection = cells();
    auto cells_collection_size = static_cast<i32>(cells_collection->length());

    // 1. If index is less than −1 or greater than the number of elements in the cells collection, then throw an "IndexSizeError" DOMException.
    if (index < -1 || index > cells_collection_size)
        return WebIDL::IndexSizeError::create(realm(), "Index is negative or greater than the number of cells");

    // 2. Let table cell be the result of creating an element given this tr element's node document, td, and the HTML namespace.
    auto& table_cell = static_cast<HTMLTableCellElement&>(*DOM::create_element(document(), HTML::TagNames::td, Namespace::HTML));

    // 3. If index is equal to −1 or equal to the number of items in cells collection, then append table cell to this tr element.
    if (index == -1 || index == cells_collection_size)
        TRY(append_child(table_cell));

    // 4. Otherwise, insert table cell as a child of this tr element, immediately before the indexth td or th element in the cells collection.
    else
        insert_before(table_cell, cells_collection->item(index));

    // 5. Return table cell.
    return JS::NonnullGCPtr(table_cell);
}

// https://html.spec.whatwg.org/multipage/tables.html#dom-tr-deletecell
WebIDL::ExceptionOr<void> HTMLTableRowElement::delete_cell(i32 index)
{
    auto cells_collection = cells();
    auto cells_collection_size = static_cast<i32>(cells_collection->length());

    // 1. If index is less than −1 or greater than or equal to the number of elements in the cells collection, then throw an "IndexSizeError" DOMException.
    if (index < -1 || index >= cells_collection_size)
        return WebIDL::IndexSizeError::create(realm(), "Index is negative or greater than or equal to the number of cells");

    // 2. If index is −1, then remove the last element in the cells collection from its parent, or do nothing if the cells collection is empty.
    if (index == -1) {
        if (cells_collection_size > 0)
            cells_collection->item(cells_collection_size - 1)->remove();
    }

    // 3. Otherwise, remove the indexth element in the cells collection from its parent.
    else {
        cells_collection->item(index)->remove();
    }

    return {};
}

}
