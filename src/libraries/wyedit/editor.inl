#ifndef EDITOR_INL
#define EDITOR_INL
#include "libraries/wyedit/editor.h"

template <typename key_type>
typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, key_type>::type>::type Editor::misc_field() const {
	return boost::fusion::at_key<key_type>(_misc_fields);
}

template <typename key_type>
void Editor::misc_field(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map const, key_type>::type>::type &value) {
	boost::fusion::at_key<key_type>(_misc_fields) = value;
}

#ifndef EDITOR_DEFINITION_GET_MISC_FIELD
#define EDITOR_DEFINITION_GET_MISC_FIELD(key_type) \
	template typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, key_type>::type>::type Editor::misc_field<key_type>() const;
#endif  // EDITOR_DEFINITION_GET_MISC_FIELD

EDITOR_DEFINITION_GET_MISC_FIELD(id_key)
EDITOR_DEFINITION_GET_MISC_FIELD(pin_key)
EDITOR_DEFINITION_GET_MISC_FIELD(rating_key)
EDITOR_DEFINITION_GET_MISC_FIELD(name_key)
EDITOR_DEFINITION_GET_MISC_FIELD(author_key)
EDITOR_DEFINITION_GET_MISC_FIELD(home_key)
EDITOR_DEFINITION_GET_MISC_FIELD(url_key)
EDITOR_DEFINITION_GET_MISC_FIELD(tags_key)
EDITOR_DEFINITION_GET_MISC_FIELD(ctime_key)
EDITOR_DEFINITION_GET_MISC_FIELD(dir_key)
EDITOR_DEFINITION_GET_MISC_FIELD(file_key)
EDITOR_DEFINITION_GET_MISC_FIELD(crypt_key)
EDITOR_DEFINITION_GET_MISC_FIELD(has_attach_key)
EDITOR_DEFINITION_GET_MISC_FIELD(attach_size_key)

#ifndef EITOR_DEFINITION_SET_MISC_FIELD
#define EITOR_DEFINITION_SET_MISC_FIELD(key_type) \
	template void Editor::misc_field<key_type>(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map const, key_type>::type>::type &value);
#endif  // EITOR_DEFINITION_SET_MISC_FIELD

EITOR_DEFINITION_SET_MISC_FIELD(id_key)
EITOR_DEFINITION_SET_MISC_FIELD(pin_key)
EITOR_DEFINITION_SET_MISC_FIELD(rating_key)
EITOR_DEFINITION_SET_MISC_FIELD(name_key)
EITOR_DEFINITION_SET_MISC_FIELD(author_key)
EITOR_DEFINITION_SET_MISC_FIELD(home_key)
EITOR_DEFINITION_SET_MISC_FIELD(url_key)
EITOR_DEFINITION_SET_MISC_FIELD(tags_key)
EITOR_DEFINITION_SET_MISC_FIELD(ctime_key)
EITOR_DEFINITION_SET_MISC_FIELD(dir_key)
EITOR_DEFINITION_SET_MISC_FIELD(file_key)
EITOR_DEFINITION_SET_MISC_FIELD(crypt_key)
EITOR_DEFINITION_SET_MISC_FIELD(has_attach_key)
EITOR_DEFINITION_SET_MISC_FIELD(attach_size_key)

#endif  // EDITOR_INL
