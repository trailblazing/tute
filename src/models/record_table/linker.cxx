
//#include "models/tree/tree_item.dec"

#include "linker.hxx"
#include "libraries/global_parameters.h"
#include "main.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "models/tree/tree_item.h"
#include "utility/delegate.h"

extern std::shared_ptr<gl_para> gl_paras;

Linker::~Linker()
{
    // boost::intrusive_ptr<TreeItem> result(nullptr);
    // if(_host_parent){
    // auto r = _host_parent->release(this);
    // assert(! r);
    ////        for(auto it : _host_parent->_child_linkers){
    ////            if(it->host()->id() == _host->id()){
    ////                _host_parent->_child_linkers.removeOne(it);// _child_items.removeAt(position);    // _child_items.takeAt(position).reset(); // delete _child_items.takeAt(position);
    ////                it->host()->page_break();
    ////                if(it->host() != _host)_host->merge(it->host());
    ////                it.reset();
    ////            }
    ////        }
    // }
    if (_host_parent)
        _host_parent->release([&](boost::intrusive_ptr<const Linker> il) { return il->host()->id() == this->host()->id() && il == this; });
    _host_parent = nullptr;
    // self_remove_from_parent_as_empty();
    if (_host) {
        // auto _p = _host->linker()->_host_parent;

        // if(_p && _p != _host_parent) {
        ////        _child_item->self_remove_from_parent();
        ////        if(_parent_item) {
        // _p->release(_host->linker());
        ////        }
        // }

        _host->linker(nullptr);
        _host = nullptr;
    }
    // return result;
}

Linker::Linker(boost::intrusive_ptr<i_t> host_parent_item, boost::intrusive_ptr<i_t> host_item) // , const int pos, const int mode
    : boost::intrusive_ref_counter<Linker, boost::thread_safe_counter>(),
      _host_parent([&]() -> boost::intrusive_ptr<i_t> {assert(host_parent_item != host_item); return host_parent_item; }()) // _new_parent
      ,
      _host(host_item // under construction linker will bind with the host and it is to be replaced
          // [&](){
          // auto older_linker = host_item->linker();	// undefined! it is the future of this linker
          // if(older_linker){
          // auto old_host_parent = older_linker->host_parent();
          // if(  old_host_parent
          // && old_host_parent != _host_parent
          // && old_host_parent->contains_direct(older_linker)	// && _old_host_parent->id() != host_item->id()
          // ){
          // old_host_parent->release([&](boost::intrusive_ptr<const Linker> il){return il->host()->id() == host_item->id();});
          // }
          // }
          // return host_item;
          // } ()
          ),
      _status([&, this] {
          auto status = sd::make_unique<status_type>();

          std::get<HOST_EXIST>(*status) = [&, this]() { return _host; }; //
          std::get<HOST_PARENT_EXIST>(*status) = [&, this]() { return _host_parent; }; //
          std::get<HOST_LINKER_EXIST>(*status) = [&, this]() { return _host->linker(); }; // std::forward < const boost::intrusive_ptr<Linker> > (this);    // you cannot forward something more than once?
          std::get<HOST_PARENT_LINKER_EXIST>(*status) = [&, this]() {auto it = _host_parent ? _host_parent->contains_direct(this) : nullptr; return it ? it->linker() : nullptr; }; //
          std::get<HOST_LINKER_IS_THIS>(*status) = [&, this]() { return _host->linker() == this; }; //
          std::get<HOST_PARENT_LINKERS_HAS_THIS>(*status) = [&, this]() {auto status_3 = [&, this](){auto it = _host_parent ? _host_parent->contains_direct(this) : nullptr; return it ? it->linker() : nullptr;}; return status_3() == this || !_host_parent; }; //
          std::get<EXTERNAL_HOST_IS_THIS_HOST>(*status) = [&, this](boost::intrusive_ptr<const i_t> host_) { return host_ == _host; }; //
          std::get<EXTERNAL_HOST_PARENT_IS_THIS_HOST_PARENT>(*status) = [&, this](boost::intrusive_ptr<const i_t> host_parent_) { return host_parent_ == _host_parent; }; //
          return status; // std::move(status);
          // return _status;
      }()),
      integrity_fix_up(
          // [&] {
          // auto integrity_fix_up_impl =
          [&](boost::intrusive_ptr<i_t> _target_parent, const int pos, const int mode) -> boost::intrusive_ptr<Linker> {
              boost::intrusive_ptr<Linker> _result(nullptr);

              // Добавление новой записи
              // Метод только добавляет во внутреннее представление новые данные,
              // сохраняет текст файла и обновляет данные на экране.
              // Сохранения дерева XML-данных на диск в этом методе нет.
              // Допустимые режимы:
              // ADD_NEW_RECORD_TO_END - в конец списка, pos игнорируется
              // ADD_NEW_RECORD_BEFORE - перед указанной позицией, pos - номер позиции
              // ADD_NEW_RECORD_AFTER - после указанной позиции, pos - номер позиции
              // Метод принимает "тяжелый" объект записи
              // Объект для вставки приходит как незашифрованным, так и зашифрованным
              auto child_move_unique = [&](boost::intrusive_ptr<i_t> _parent_target, int pos = 0, int mode = add_new_record_after) -> boost::intrusive_ptr<Linker> { // , boost::intrusive_ptr<TreeItem>   _host   // child_insert? does not set parent pointer?
                  boost::intrusive_ptr<Linker> result(nullptr);
                  // boost::intrusive_ptr<TreeItem> _child_item(this);
                  // assert(_self != _parent);
                  assert(_host != _parent_target);
                  if (_parent_target && _host != _parent_target) {
                      int actually_insert_position = -1;
                      // int result_should_be_position = pos;
                      int count = 0;
                      int found = 0;
                      if (!integrity_external(this->_host, _parent_target)) {
                          // if(_host_parent != _parent){
                          if (_host_parent && _host_parent != _parent_target && _host_parent->contains_direct(_host))
                              _host_parent->release([&](boost::intrusive_ptr<const Linker> il) { return il->host()->id() == this->host()->id() && il == this; });
                          for (auto il : _parent_target->_child_linkers) { // = _parent->_child_linkers.begin(); it != _parent->_child_linkers.end(); it++) {
                              if (il == this) {
                                  found++;
                                  if (found == 1) {
                                      result = this;
                                      this->_host_parent = _parent_target;
                                      actually_insert_position = count;
                                      integrity_external(_host, _host_parent);
                                  } else
                                      _parent_target->_child_linkers.removeOne(il);
                              }
                              count++;
                          }
                          // if(0 == found) {
                          // Запись добавляется в таблицу конечных записей
                          // get shadow_branch
                          // TreeModelKnow *shadow_branch = globalparameters.tree_screen()->_shadow_branch;  // static_cast<TreeModelKnow *>(find_object<TreeViewKnow>(knowtreeview_singleton_name)->model());
                          if (0 == found) { // !item_direct(_source_item)
                              if (_host_parent != _parent_target)
                                  _host_parent = _parent_target;
                              // && !shadow_branch->is_record_id_exists(item->field("id"))
                              //// deprecated by _parent_target->remove
                              // auto _origin_parent = _source_item->parent();
                              // if(_origin_parent && _origin_parent != _parent_target) {
                              ////                    if(_parent != this) {
                              // _origin_parent->remove(_source_item);
                              ////                    while(_origin_parent->remove_self_as_empty()) {
                              ////                        _origin_parent = _origin_parent->parent();
                              ////                        if(!_origin_parent)break;
                              ////                    }
                              ////                    }
                              // }
                              // !!!, this make item move to new branch, remember to remove orignal one
                              // _child_item->_parent_item = _parent_target; // _source_item->parent(boost::intrusive_ptr<TreeItem>(_parent_target));    //_item->parent(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)));
                              // if(_host->up_linker() != this)_host->up_linker(this);
                              // if(_tree_item) qDebug() << "RecordTable::insert_new_record() : Insert new record to branch " << _tree_item->all_fields();
                              // Выясняется, есть ли в дереве запись с указанным ID
                              // Если есть, то генерируются новые ID для записи и новая директория хранения
                              // Если нет, то это значит что запись была вырезана, но хранится в буфере,
                              // и ее желательно вставить с прежним ID и прежним именем директории
                              // It turns out, is there a record of a tree with specified ID
                              // If there is, then the generated ID for the new record and the new storage directory
                              // If not, then it means that the recording was cut out, but stored in a buffer
                              // And it is desirable to stick with the same ID and the same name directory
                              // KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>(knowtreeview_singleton_name)->model());
			      if (0 == detail::to_string(_host->field<id_type>()).length()) { // || dataModel->isRecordIdExists(record->getField("id"))  // ? Is this a correct policy? can we only create a pure view?
                                  // Создается новая запись (ID был пустой) или
                                  // Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)
                                  // Create a new record (ID was empty) or
                                  // Record with ID in a tree there, so stand the new ID and a new storage directory (not to overwrite existing)

				  QString id = detail::to_string(_host->field<dir_type>()).length() > 0 ? _host->field<dir_type>() : get_unical_id();
                                  // Store directory entries and files    // Директория хранения записи и файл
				  if (detail::to_string(_host->field<dir_type>()) == "")
				      _host->field<dir_type>(dir_value(id)); // get_unical_id()
				  if (detail::to_string(_host->field<file_type>()) == "")
				      _host->field<file_type>(file_value("text.html"));
                                  // Unique ID of XML records             // Уникальный идентификатор XML записи
                                  // QString id = get_unical_id();
				  _host->field<id_type>(id_value(id));
                              } else {
				  if (detail::to_string(_host->field<dir_type>()) == "")
				      _host->field<dir_type>(dir_value(detail::to_string(_host->field<id_type>()))); // get_unical_id()
				  if (detail::to_string(_host->field<file_type>()) == "")
				      _host->field<file_type>(file_value("text.html"));
                              }
                              // The method must take a full-fledged object record    // Мотод должен принять полновесный объект записи
                              if (_host->is_lite())
                                  _host->to_fat(); // critical_error("ItemsFlat::insert_new_item() can't insert lite record");
                              // В список переданных полей добавляются вычислимые в данном месте поля
			      if (detail::to_string(_host->field<ctime_type>()) == "") {
                                  // Время создания данной записи
                                  // QDateTime ctime_dt = QDateTime::currentDateTime();

                                  QString ctime = get_qtime(); // ctime_dt.toString("yyyyMMddhhmmss");
				  _host->field<ctime_type>(ctime_value(ctime));
                              }
                              // Выясняется в какой ветке вставляется запись - в зашифрованной или нет
                              bool is_crypt = false;
                              // if(_tree_item) {
                              // if(_is_crypt) { //
			      if (detail::to_string(_host_parent->field<crypt_type>()) == "1") {
                                  if (gl_paras->crypt_key().length() > 0)
                                      is_crypt = true;
                                  else
                                      critical_error("Linker::child_move_unique() : Can not insert data to crypt branch. Password not setted.");
                              }
                              // }
                              // Запись полновесных данных с учетом шифрации
			      if (is_crypt && detail::to_string(_host->field<crypt_type>()) != "1") // В зашифрованную ветку незашифрованную запись
                                  _host->to_encrypt_and_save_fat();
			      else if (!is_crypt && detail::to_string(_host->field<crypt_type>()) == "1") // В незашифрованную ветку зашифрованную запись
                                  _host->to_decrypt_and_save_fat();
                              else
                                  _host->push_fat_attributes();
                              // Record switch to easy mode to be added to the final table of records ?***    // Запись переключается в легкий режим чтобы быть добавленной в таблицу конечных записей
                              _host->to_lite();
                              ////        // Запись добавляется в таблицу конечных записей
                              ////        int insertPos = -1;
                              // item->is_registered_to_record_controller_and_tabmanager(true);
                              if (mode == add_new_record_to_end) { // В конец списка           // mode == 0
                                  _host_parent->_child_linkers << this; // _self;
                                  // result_should_be_position = _parent->_child_linkers.size() - 1;	// parent may be empty before
                              } else if (mode == add_new_record_before) { // Перед указанной позицией // mode == 1
                                  _host_parent->_child_linkers.insert(pos, this);
                                  // result_should_be_position = pos < 0 ? 0 : pos;
                              } else if (mode == add_new_record_after) { // После указанной позиции  // mode == 2
                                  // auto last = _parent->_child_linkers.size() - 1;
                                  _host_parent->_child_linkers.insert(pos + 1, this);
                                  // result_should_be_position = pos + 1 > last + 1 ? last + 1 : pos + 1 < 0 ? 0 : pos + 1;	// parent may be empty before
                              }
                              // if(this->_host_parent != _parent) this->_host_parent = _parent;
                              // insert_position =  _parent->sibling_order(_self);  //_child_item->record_linker()->sibling_order();   // ItemsFlat::
                              // _parent_target->sibling_order(_source_item);
                              // qDebug() << "ItemsFlat::insert_new_item() : New record pos" << QString::number(insert_position);

                              // Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода
                              actually_insert_position = _host_parent->sibling_order([&](boost::intrusive_ptr<const Linker> il) { return il == this && il == _host->linker() && il->host() == _host; });
                              if (_host_parent && !_host_parent->contains_direct(this))
                                  throw std::runtime_error("child_move_unique : _host_parent && _host_parent->contains_direct(this) failure");
                              if (!integrity_external(_host, _host_parent))
                                  throw std::runtime_error("child_move_unique : integrity_external failure");
                          }
                          // else {
                          // insert_position = _parent->sibling_order(_self);  // _child_item->sibling_order();    // ItemsFlat::
                          ////                    _parent_target->sibling_order(_source_item);
                          // }

                          // int insert_position = _host_parent->sibling_order([&](boost::intrusive_ptr<const Linker> il) {return il == _host->linker() && il->host() == _host;});

                          // if(mode == add_new_record_after)result_should_be_position = _host_parent->count_direct() == 1 ? pos : pos + 1; // parent may be empty before
                          // else if(mode == add_new_record_to_end)result_should_be_position = _host_parent->count_direct() - 1; // parent may be empty before

                          // assert(result_should_be_position == actually_insert_position);

                          // qDebug() << "Linker::child_move_unique() : New record pos" << QString::number(actually_insert_position);
                          assert(_host_parent->_child_linkers[actually_insert_position] == this);

                          result = std::get<HOST_PARENT_LINKER_EXIST>(*_status)(); // _host_parent->_child_linkers.contains(this);
                          // if(result){
                          ////				  result = this;

                          ////				  auto item = _host_parent ? _host_parent->contains_direct(this) : nullptr;
                          ////				  auto linker_ = item ? item->linker() : nullptr;
                          ////				  assert(linker_ == this);
                          ////				  auto linker = std::get<HOST_PARENT_LINKER_EXIST>(*_status)();
                          // assert(this == std::get<HOST_PARENT_LINKER_EXIST>(*_status)());
                          // }else{
                          // assert(false);
                          //// assert(std::get<HOST_PARENT_LINKER_EXIST>(_status)());
                          // }
                          // }
                          assert(result);
			  if (boost::fusion::at_key<crypt_type>(_host_parent->_field_data_map) == crypt_value(true)) { // if(_host_parent->_field_data[boost::mpl::c_str < crypt_type > ::value] == "1"){ // new_host_parent &&
			      if (boost::fusion::at_key<crypt_type>(_host->_field_data_map) != crypt_value(true))
                                  _host->to_encrypt();
			  } else if (boost::fusion::at_key<crypt_type>(_host->_field_data_map) == crypt_value(true))
                              _host->to_decrypt();
                          // }
                          // integrity_fix_up();   // recursive call
                      }
                      assert(_parent_target == _host_parent);
                      if (!_host_parent)
                          throw std::runtime_error("child_move_unique : nullptr == _host_parent");
                      if (!_host_parent->contains_direct(this))
                          throw std::runtime_error("child_move_unique : _host_parent && _host_parent->contains_direct(this) failure");
                      if (!integrity_external(_host, _host_parent))
                          throw std::runtime_error("child_move_unique : integrity_external failure");
                  }
                  // else{
                  // throw std::runtime_error("child_move_unique : nullptr == _parent");		// std::exception();
                  // }
                  // else {
                  // result = boost::intrusive_ptr<TreeItem>(this);
                  // }

                  return result; // insert_position;
              };
              bool result = true;
              // if(_parent != _host_parent)_host_parent = _parent;
              if (!std::get<HOST_EXIST>(*_status)()) {
                  result = false;
                  assert(result);
              }
              if (!std::get<HOST_PARENT_EXIST>(*_status)()) {
                  if (_host_parent)
                      result = false;
                  assert(result);
              }
              if (std::get<HOST_LINKER_EXIST>(*_status)() != nullptr) { // !std::get<2>(_status)()   // change rvalue to lvalue!!! // host()->linker(); // _host is under construction at this time
                  _host->linker(this); // change rvalue to lvalue!!!
                  if (!_host->linker()) {
                      result = false;
                      assert(result);
                  }
              }
              if (!std::get<HOST_PARENT_LINKER_EXIST>(*_status)() || _target_parent != _host_parent) { // (_parent != _host_parent)// to merge
                  _result = child_move_unique(_target_parent, pos, mode);
                  assert(_target_parent == _host_parent);
                  if (_result && _host_parent) {
                      if (!_host_parent->contains_direct(this)) {
                          result = false;
                          assert(result);
                      }
                      if (!std::get<HOST_LINKER_IS_THIS>(*_status)()) {
                          result = false;
                          assert(result);
                      }
                      if (!std::get<HOST_PARENT_LINKERS_HAS_THIS>(*_status)()) {
                          result = false;
                          assert(result);
                      }
                  }
              } else
                  assert(_target_parent == _host_parent);
              return _result;
          }
          // ;
          // integrity_fix_up_impl(_host_parent, pos, mode);
          // return integrity_fix_up_impl;
          // } ()
          )
{
    //

    // [&] {

    //// status_type status;

    // std::get<0>(_status) = [&](){return host();};																															//
    // std::get<1>(_status) = [&](){return host_parent();};																														//
    // std::get<2>(_status) = [&](){return _host->linker();};																														// std::forward < const boost::intrusive_ptr<Linker> > (this);    // you cannot forward something more than once?
    // std::get<3>(_status) = [&](){auto it = _host_parent ? _host_parent->contains_direct(this) : nullptr;return it ? it->linker() : nullptr;};																														//
    // std::get<4>(_status) = [&](){return _host->linker() == this;};																															//
    // std::get<5>(_status) = [&](){return std::get<3>(_status)() == this || ! _host_parent;};																																	//
    // std::get<6>(_status) = [&](boost::intrusive_ptr<const TreeItem> host_){return host_ == _host;};																																				//
    // std::get<7>(_status) = [&](boost::intrusive_ptr<const TreeItem> host_parent_){return host_parent_ == _host_parent;};																																						//

    //// return std::move(status);
    //// return _status;
    // } ();

    ////    auto internal_contain_check = [](boost::intrusive_ptr<TreeItem> parent_, linker * const this_)->linker* {
    ////        linker *result = nullptr;

    ////        for(auto il : parent_->_child_linkers)
    ////        {
    ////            //
    ////        }

    ////        return result;
    ////    };

    ////    [ & ]()->boost::intrusive_ptr<TreeItem> {
    ////        this->_parent_item = _parent;
    ////        this->_child_item = _child_item;

    // boost::intrusive_ptr<Linker> _result(nullptr);

    // if(_host_parent) {
    // if(!_host_parent->contains_direct(std::forward < const boost::intrusive_ptr<Linker> > (this)) // _host->up_linker()   // _host is under contruction at this time
    // && _host_parent->id() != _host->_field_data.value("id")) {

    ////        this->child_move_unique =

    ////    if(_parent_item && _parent_item != _new_parent) {
    ////        _parent_item->remove(_child_item);
    ////    }

    ////    if(_parent_item && _parent_item != _parent) {
    ////        _parent_item->remove(_child_item);
    ////    }

    ////        auto _child_item_parent = _child_item->parent();  // form ItemsFlat, incomplete

    ////        if(_child_item_parent && _child_item_parent != _parent_item) {
    ////            _child_item_parent->remove(_child_item);
    ////        }

    ////        if(_parent) {

    ////        assert(_parent_item->id() != _child_item->_field_data.value("id"));   //        assert(_parent_item->name() != _field_data.value("name"));

    //// _parent_item->add_child(boost::intrusive_ptr<TreeItem>(this));

    ////            //        }

    ////            //    _parent_item = _new_parent;

    ////            //            if(_parent) {    //  && _parent_item != it
    ////            //        _parent_item = it;
    ////            bool found = false;

    ////            for(auto i : _parent_item->_child_items) {
    ////                if(_child_item->id() == i->id()) {found = true; break;}
    ////            }

    ////            if(!found) {
    ////                //            _result = _parent_item->child_move_unique(this
    ////                //                                                      , _pos  // _parent_item->count_direct() > 0 ? _parent_item->count_direct() - 1 : 0
    ////                //                                                      , _mode
    ////                //                                                     ); // _parent_item->_child_items << boost::intrusive_ptr<TreeItem>(this);

    // _result = child_move_unique(_host_parent, pos, mode);    // , _host

    //// _parent_item->count_direct() > 0 ? _parent_item->count_direct() - 1 : 0
    //// _parent_item->_child_items << boost::intrusive_ptr<TreeItem>(this);
    ////            }

    // if(_host_parent->_field_data[boost::mpl::c_str < crypt_type > ::value] == "1") {
    // _host->to_encrypt();
    // } else {
    // _host->to_decrypt();
    // }

    ////        }

    ////        }

    ////        if(_parent_item && _parent_item->up_linker() != this)_parent_item->up_linker(this); // deprecated ItemsFlat::_child_items move to ItemsFlat::_child_linkers

    // }
    // }

    ////    auto is_integratied = integrity();
    ////    assert(is_integratied);

    ////    //    //    return // _parent_item =
    ////    //    //        _parent;
    ////    //    //    }();

    ////    //    //    [&]()->boost::intrusive_ptr<TreeItem> {
    ////    //    _child_item->record_linker(this);
    ////    //    //        return _child_item;
    ////    //    //    }();  //

    ////    //    //    return _result;
}

int Linker::sibling_order() const
{
    int result = -1;
    if (_host_parent)
        result = _host_parent->_child_linkers.indexOf(_host->linker()); // boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()

    return result;
}

int Linker::move_up(void)
{
    int row = 0;
    // Выясняется номер данного элемента в списке родителя
    int num = sibling_order();
    if (num > 0 && num < (_host_parent->count_direct())) { // if(num == 0)return false;    // Если двигать вверх некуда, ничего делать ненужно
        row = -1;
        // Элемент перемещается вверх по списку
        (_host_parent->_child_linkers).swap(num, num + row);
    }
    return row;
}

int Linker::move_dn(void)
{
    int row = 0;
    // Выясняется номер данного элемента в списке родителя
    int num = sibling_order();
    // Если двигать вниз некуда, ничего делать ненужно
    if (num > 0 && num < (_host_parent->count_direct() - 1)) { // if(num >= (_parent_item->count_direct()))return false;
        row = 1;
        // Элемент перемещается вниз по списку
        (_host_parent->_child_linkers).swap(num, num + row);
    }
    return row;
}

boost::intrusive_ptr<i_t> Linker::host_parent() const { return _host_parent; }

boost::intrusive_ptr<i_t> Linker::host() const { return _host; }

void Linker::host_parent(boost::intrusive_ptr<i_t>&& p) { _host_parent = std::forward<boost::intrusive_ptr<i_t> >(p); }

void Linker::host(boost::intrusive_ptr<i_t>&& h) { _host = std::forward<boost::intrusive_ptr<i_t> >(h); }

boost::intrusive_ptr<Linker> Linker::parent(boost::intrusive_ptr<i_t> _parent, const int pos, const int mode)
{
    assert(_host);
    boost::intrusive_ptr<Linker> result(nullptr);
    // if(_parent){
    // if(! integrity_external(this->_host, parent_item) && parent_item->id() != _host->_field_data.value("id")){		// parent_item != _host_parent && !parent_item->contains_direct(std::forward < const boost::intrusive_ptr<linker> > (this)) // || // _host->up_linker()

    assert(_host->linker() == this);
    assert(_host->linker()->host_parent() == this->_host_parent);
    // auto _parent_item = _host->up_linker()->parent_item();
    // if(_host_parent){
    // if(  _host_parent != parent_item
    // && _host_parent->contains_direct(this)// std::forward < const boost::intrusive_ptr<const Linker> > (this)// _host->up_linker()
    // && _host_parent->id() != _host->_field_data.value("id")
    // ){
    // _host_parent->release([&](boost::intrusive_ptr<const Linker> il){return il->host()->id() == this->host()->id() && il == this;});	// _parent_item->delete_permanent(_host->up_linker());    // leading boost::intrusive_ptr<linker> reseted!!!, so this function may be an illegal operation?
    // _host_parent = parent_item;
    // _host_parent->_child_linkers.insert(pos, this);
    // }
    // result = child_move_unique(_parent, pos, mode);	// , _host
    // if(_host_parent->_field_data[boost::mpl::c_str < crypt_type > ::value] == "1"){													// new_host_parent &&
    // if(_host->_field_data[boost::mpl::c_str < crypt_type > ::value] != "1")_host->to_encrypt();
    // }else{
    // if(_host->_field_data[boost::mpl::c_str < crypt_type > ::value] == "1")_host->to_decrypt();
    // }
    // }
    if (_host == _parent)
        throw std::runtime_error("try to point to self as parent");
    // _host_parent = new_host_parent;
    result = integrity_fix_up(_parent, pos, mode);
    assert(_parent == _host_parent);
    assert(integrity_external(this->_host, _host_parent) || !_parent); //

    // int actually_insert_position = _host_parent->sibling_order([&](boost::intrusive_ptr<const Linker> il) {return il == _host->linker() && il->host() == _host;});
    // int result_should_be_position = pos;

    // if(mode == add_new_record_after)result_should_be_position = _host_parent->count_direct() == 1 ? pos : pos + 1; // parent may be empty before
    // else if(mode == add_new_record_to_end)result_should_be_position = _host_parent->count_direct() - 1; // parent may be empty before

    // assert(result_should_be_position == actually_insert_position);
    // }

    // }
    return result;
}

void Linker::dangle()
{
    // if(_host_parent->up_linker())_host_parent->up_linker(nullptr);    // pretty wrong semantic!
    _host->dangle();

    // if(_host->up_linker()) {
    //// boost::intrusive_ptr<TreeItem> _parent = _host->up_linker()->_host_parent;
    //// _parent = nullptr;
    // _host->up_linker().reset();
    // }
}

bool Linker::integrity_internal() const
{
    return std::get<HOST_EXIST>(*_status)()
        && std::get<HOST_PARENT_EXIST>(*_status)()
        && std::get<HOST_LINKER_EXIST>(*_status)()
        && std::get<HOST_PARENT_LINKER_EXIST>(*_status)()
        && std::get<HOST_LINKER_IS_THIS>(*_status)()
        && std::get<HOST_PARENT_LINKERS_HAS_THIS>(*_status)();
}

bool Linker::integrity_external(boost::intrusive_ptr<const i_t> host_, boost::intrusive_ptr<const i_t> host_parent_) const
{
    return integrity_internal()
        && std::get<EXTERNAL_HOST_IS_THIS_HOST>(*_status)(host_)
        && std::get<EXTERNAL_HOST_PARENT_IS_THIS_HOST_PARENT>(*_status)(host_parent_);
}

// Linker::status_type Linker::state_impl()
// {
// status_type status;
// std::get<0>(status) = [&](){return host();};																															//
// std::get<1>(status) = [&](){return host_parent();};																														//
// std::get<2>(status) = [&](){return _host->linker();};																														// std::forward < const boost::intrusive_ptr<Linker> > (this);    // you cannot forward something more than once?
// std::get<3>(status) = [&](){auto it = _host_parent ? _host_parent->contains_direct(this) : nullptr;return it ? it->linker() : nullptr;};																														//
// std::get<4>(status) = [&](){return _host->linker() == this;};																															//
// std::get<5>(status) = [&](){return std::get<3>(status)() == this || ! _host_parent;};																																	//
// std::get<6>(status) = [&](boost::intrusive_ptr<const TreeItem> host_){return host_ == _host;};																																				//
// std::get<7>(status) = [&](boost::intrusive_ptr<const TreeItem> host_parent_){return host_parent_ == _host_parent;};

// return std::move(status);
// }   // is_closure

// bool Linker::integrity()
// {
// bool result = true;

// if(!std::get<0>(_status)())result = false;

// if(!std::get<1>(_status)()) {if(_host_parent)result = false;}

// if(// !std::get<2>(_status)()   // change rvalue to lvalue!!!
////        host()->up_linker(); // _host is under construction at this time
// std::get<2>(_status)() != nullptr) {
// _host->up_linker(std::forward<boost::intrusive_ptr<linker>>(this));   // change rvalue to lvalue!!!

// if(!_host->up_linker())   // change rvalue to lvalue!!!
// result = false;
// }

// if(!std::get<3>(_status)()) {
// this->child_move_unique(_host_parent, 0, ADD_NEW_RECORD_BEFORE);

// if(_host_parent) {
// if(!_host_parent->contains_direct(std::forward < const boost::intrusive_ptr<linker> > (this)))
// result = false;
// }
// }

// return result;
// };

boost::intrusive_ptr<Linker> Linker::instance(boost::intrusive_ptr<i_t> _host, boost::intrusive_ptr<i_t> _parent, int pos, int mode)
{ // we can use caller temmplate to get caller info
    // boost::intrusive_ptr<TreeItem> _result(nullptr);
    //// Добавление новой записи
    //// Метод только добавляет во внутреннее представление новые данные,
    //// сохраняет текст файла и обновляет данные на экране.
    //// Сохранения дерева XML-данных на диск в этом методе нет.
    //// Допустимые режимы:
    //// ADD_NEW_RECORD_TO_END - в конец списка, pos игнорируется
    //// ADD_NEW_RECORD_BEFORE - перед указанной позицией, pos - номер позиции
    //// ADD_NEW_RECORD_AFTER - после указанной позиции, pos - номер позиции
    //// Метод принимает "тяжелый" объект записи
    //// Объект для вставки приходит как незашифрованным, так и зашифрованным
    // auto child_move_unique = [&](boost::intrusive_ptr<TreeItem> _parent_target, int pos, int mode)->boost::intrusive_ptr<TreeItem> { // child_insert? does not set parent pointer?
    // boost::intrusive_ptr<TreeItem> result(nullptr);
    // boost::intrusive_ptr<TreeItem> _source_item(this);
    // assert(_source_item != _parent_target);
    // if(_source_item != _parent_target)
    // {
    // int found = 0;
    // for(auto it = _parent_target->_child_items.begin(); it != _parent_target->_child_items.end(); it++) {
    // if(it->get() == _source_item.get()) {
    // found++;
    // if(found == 1)
    // result = *it;
    // else
    // _parent_target->_child_items.erase(it);
    // }
    // }
    ////        if(0 == found) {
    //// Запись добавляется в таблицу конечных записей
    // int insert_position = -1;
    //// get shadow_branch
    ////    TreeModelKnow *shadow_branch = globalparameters.tree_screen()->_shadow_branch;  // static_cast<TreeModelKnow *>(find_object<TreeViewKnow>(knowtreeview_singleton_name)->model());
    // if(
    // 0 == found  // !item_direct(_source_item)
    // ) {  //       && !shadow_branch->is_record_id_exists(item->field("id"))
    ////                // deprecated by _parent_target->remove
    ////                auto _origin_parent = _source_item->parent();
    ////                if(_origin_parent && _origin_parent != _parent_target) {
    ////                    //                    if(_parent != this) {
    ////                    _origin_parent->remove(_source_item);
    ////                    //                    while(_origin_parent->remove_self_as_empty()) {
    ////                    //                        _origin_parent = _origin_parent->parent();
    ////                    //                        if(!_origin_parent)break;
    ////                    //                    }
    ////                    //                    }
    ////                }
    //// !!!, this make item move to new branch, remember to remove orignal one
    // _source_item->_parent_item = _parent_target; // _source_item->parent(boost::intrusive_ptr<TreeItem>(_parent_target));    //_item->parent(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)));
    ////        if(_tree_item) qDebug() << "RecordTable::insert_new_record() : Insert new record to branch " << _tree_item->all_fields();
    //// Выясняется, есть ли в дереве запись с указанным ID
    //// Если есть, то генерируются новые ID для записи и новая директория хранения
    //// Если нет, то это значит что запись была вырезана, но хранится в буфере,
    //// и ее желательно вставить с прежним ID и прежним именем директории
    //// It turns out, is there a record of a tree with specified ID
    //// If there is, then the generated ID for the new record and the new storage directory
    //// If not, then it means that the recording was cut out, but stored in a buffer
    //// And it is desirable to stick with the same ID and the same name directory
    ////        KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>(knowtreeview_singleton_name)->model());
    // if(0 == _source_item->field("id").length()) { //           || dataModel->isRecordIdExists(record->getField("id"))  // ? Is this a correct policy? can we only create a pure view?
    //// Создается новая запись (ID был пустой) или
    //// Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)
    //// Create a new record (ID was empty) or
    //// Record with ID in a tree there, so stand the new ID and a new storage directory (not to overwrite existing)
    // QString id = get_unical_id();
    //// Store directory entries and files    // Директория хранения записи и файл
    // if(_source_item->field("dir") == "")_source_item->field("dir", id); // get_unical_id()
    // if(_source_item->field("file") == "")_source_item->field("file", "text.html");
    //// Unique ID of XML records             // Уникальный идентификатор XML записи
    ////            QString id = get_unical_id();
    // _source_item->field("id", id);
    // } else {
    // if(_source_item->field("dir") == "")_source_item->field("dir", _source_item->field("id")); // get_unical_id()
    // if(_source_item->field("file") == "")_source_item->field("file", "text.html");
    // }
    //// The method must take a full-fledged object record    // Мотод должен принять полновесный объект записи
    // if(_source_item->is_lite()) {
    // _source_item->to_fat();
    ////            critical_error("ItemsFlat::insert_new_item() can't insert lite record");
    // }
    //// В список переданных полей добавляются вычислимые в данном месте поля
    // if(_source_item->field("ctime") == "") {
    //// Время создания данной записи
    // QDateTime ctime_dt = QDateTime::currentDateTime();
    // QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
    // _source_item->field("ctime", ctime);
    // }
    //// Выясняется в какой ветке вставляется запись - в зашифрованной или нет
    // bool is_crypt = false;
    ////        if(_tree_item) {
    ////                if(_is_crypt) { //
    // if(_parent_target->field(boost::mpl::c_str < crypt_type > ::value) == "1") {
    // if(globalparameters.crypt_key().length() > 0)
    // is_crypt = true;
    // else
    // critical_error("ItemsFlat::insert_new_item() : Can not insert data to crypt branch. Password not setted.");
    // }
    ////        }
    //// Запись полновесных данных с учетом шифрации
    // if(is_crypt && _source_item->field(boost::mpl::c_str < crypt_type > ::value) != "1") {       // В зашифрованную ветку незашифрованную запись
    // _source_item->to_encrypt_and_save_fat();
    // } else if(!is_crypt && _source_item->field(boost::mpl::c_str < crypt_type > ::value) == "1") { // В незашифрованную ветку зашифрованную запись
    // _source_item->to_decrypt_and_save_fat();
    // } else {
    // _source_item->push_fat_attributes();
    // }
    //// Record switch to easy mode to be added to the final table of records ?***    // Запись переключается в легкий режим чтобы быть добавленной в таблицу конечных записей
    // _source_item->to_lite();
    ////        //        // Запись добавляется в таблицу конечных записей
    ////        //        int insertPos = -1;
    ////        item->is_registered_to_record_controller_and_tabmanager(true);
    // if(mode == ADD_NEW_RECORD_TO_END) {         // В конец списка
    // _parent_target->_child_items << _source_item;
    ////            insert_position = _child_items.size() - 1;
    // } else if(mode == ADD_NEW_RECORD_BEFORE) {  // Перед указанной позицией
    // _parent_target->_child_items.insert(pos, _source_item);
    ////            insert_position = pos;
    // } else if(mode == ADD_NEW_RECORD_AFTER) {   // После указанной позиции
    // _parent_target->_child_items.insert(pos + 1, _source_item);
    ////            insert_position = pos + 1;
    // }
    // insert_position =  _source_item->sibling_order();   // ItemsFlat::
    ////                    _parent_target->sibling_order(_source_item);
    // qDebug() << "ItemsFlat::insert_new_item() : New record pos" << QString::number(insert_position);
    //// Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода
    // } else {
    // insert_position = _source_item->sibling_order();    // ItemsFlat::
    ////                    _parent_target->sibling_order(_source_item);
    // }
    // assert(_parent_target->_child_items[insert_position] == _source_item);
    // if(_parent_target->_child_items.contains(_source_item))result = _source_item;
    ////        }
    // }
    ////    else {
    ////        result = boost::intrusive_ptr<TreeItem>(this);
    ////    }
    // return result;    // insert_position;
    // };
    ////    if(_parent_item && _parent_item != it) {
    ////        _parent_item->remove(this);
    ////    }
    // if(_parent_item && _parent_item != it) {
    // _parent_item->remove(this);
    // }
    // _parent_item = it;
    // if(it) {    //  && _parent_item != it
    ////        _parent_item = it;
    // bool found = false;
    // for(auto i : _parent_item->_child_items) {
    // if(this->id() == i->id()) {found = true; break;}
    // }
    // if(!found) {
    ////            _result = _parent_item->child_move_unique(this
    ////                                                      , _pos  // _parent_item->count_direct() > 0 ? _parent_item->count_direct() - 1 : 0
    ////                                                      , _mode
    ////                                                     ); // _parent_item->_child_items << boost::intrusive_ptr<TreeItem>(this);
    // _result = child_move_unique(_parent_item
    // , _pos  // _parent_item->count_direct() > 0 ? _parent_item->count_direct() - 1 : 0
    // , _mode
    // ); // _parent_item->_child_items << boost::intrusive_ptr<TreeItem>(this);
    // }
    // }
    // if(!_up_linker || _up_linker->host_parent() != it) {
    auto _linker = _host->linker();
    if (_parent) {
        if (!_linker) {
            _host->linker(boost::intrusive_ptr<Linker>(new Linker(_parent, _host))); // , pos, mode
            _linker = _host->linker();
            auto link_result = _linker->parent(_parent, pos, mode);
            assert(link_result == _linker);
            assert(_linker->integrity_external(_host, _parent));
        } else if (_linker && _linker->host_parent() != _parent) {
            auto parent = _linker->host_parent();
            if (parent && parent != _parent)
                parent->release([&](boost::intrusive_ptr<const Linker> il) { return il->host()->id() == _host->id() && il == _linker; });
            if (_linker->host() != _host)
                _linker->host(std::forward<boost::intrusive_ptr<i_t> >(_host)); // std::move(boost::intrusive_ptr<TreeItem>(this))
            auto link_result = _linker->parent(_parent, pos, mode); // _linker->host_parent()->release(this->linker());
            assert(link_result == _linker);
            assert(_linker->integrity_external(_host, _parent));
        }
        // }
    }
    // _result = _up_linker->host();

    return _linker;
}
