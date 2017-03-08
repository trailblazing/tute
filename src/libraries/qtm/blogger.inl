#ifndef BLOGGER_INL
#define BLOGGER_INL

#include "libraries/qtm/blogger.h"
#include "libraries/qtm/qij_search_widget.h"

#if !defined DONT_USE_PTE

#include <QPlainTextEdit>
class QPlainTextEdit;

#else  //DONT_USE_PTE

#ifdef USE_WYEDIT

#ifdef USE_EDITOR_WRAP
#include "views/record/editor_wrap.h"
class EditorWrap;
//#define TEXTEDIT_FOR_READ QTextEdit
class EditorWrap;
//#define TEXTEDIT EditorWrap
typedef EditorWrap TEXTEDIT;
#else  //USE_EDITOR_WRAP

#include "libraries/wyedit/editor.h"
class Editor;
typedef Editor TEXTEDIT;

#endif  // USE_EDITOR_WRAP

#else  //USE_WYEDIT

#include <QTextEdit>

#endif  //USE_WYEDIT

#endif  //DONT_USE_PTE

template <typename key_type>
typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, key_type>::type>::type Blogger::misc_field() const {
	return _editor->misc_field<key_type>();
}

template <typename key_type>
void Blogger::misc_field(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map const, key_type>::type>::type &value) {
	_editor->misc_field<key_type>(value);
}

#ifndef DEFINITION_GET_MISC_FIELD
#define DEFINITION_GET_MISC_FIELD(key_type)                                                                                     \
	template typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, key_type>::type>::type \
	Blogger::misc_field<key_type>() const;
#endif  // DEFINITION_GET_MISC_FIELD

DEFINITION_GET_MISC_FIELD(id_key)
DEFINITION_GET_MISC_FIELD(pin_key)
DEFINITION_GET_MISC_FIELD(rating_key)
DEFINITION_GET_MISC_FIELD(name_key)
DEFINITION_GET_MISC_FIELD(author_key)
DEFINITION_GET_MISC_FIELD(home_key)
DEFINITION_GET_MISC_FIELD(url_key)
DEFINITION_GET_MISC_FIELD(tags_key)
DEFINITION_GET_MISC_FIELD(ctime_key)
DEFINITION_GET_MISC_FIELD(dir_key)
DEFINITION_GET_MISC_FIELD(file_key)
DEFINITION_GET_MISC_FIELD(crypt_key)
DEFINITION_GET_MISC_FIELD(has_attach_key)
DEFINITION_GET_MISC_FIELD(attach_size_key)

#ifndef DEFINITION_SET_MISC_FIELD
#define DEFINITION_SET_MISC_FIELD(key_type) \
	template void Blogger::misc_field<key_type>(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map const, key_type>::type>::type &value);
#endif  // DEFINITION_SET_MISC_FIELD

DEFINITION_SET_MISC_FIELD(id_key)
DEFINITION_SET_MISC_FIELD(pin_key)
DEFINITION_SET_MISC_FIELD(rating_key)
DEFINITION_SET_MISC_FIELD(name_key)
DEFINITION_SET_MISC_FIELD(author_key)
DEFINITION_SET_MISC_FIELD(home_key)
DEFINITION_SET_MISC_FIELD(url_key)
DEFINITION_SET_MISC_FIELD(tags_key)
DEFINITION_SET_MISC_FIELD(ctime_key)
DEFINITION_SET_MISC_FIELD(dir_key)
DEFINITION_SET_MISC_FIELD(file_key)
DEFINITION_SET_MISC_FIELD(crypt_key)
DEFINITION_SET_MISC_FIELD(has_attach_key)
DEFINITION_SET_MISC_FIELD(attach_size_key)

#endif  // BLOGGER_INL
