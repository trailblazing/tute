#include "linker.hxx"
#include "models/tree/TreeItem.h"
#include "models/record_table/RecordModel.h"
#include "libraries/GlobalParameters.h"
#include "main.h"

extern GlobalParameters globalparameters;

Linker::~Linker()
{


    //    boost::intrusive_ptr<TreeItem> result(nullptr);

    if(_host) {
        //        auto _p = _host->up_linker()->_parent_item;

        //        if(_p && _p != _parent_item) {
        //            //        _child_item->self_remove_from_parent();
        //            //        if(_parent_item) {
        //            //            result =
        //            _p->delete_permanent(_host->up_linker());
        //            //        }

        //        }

        //        //        _child_item->_record_linker = nullptr;

        _host = nullptr;
    }

    //    if(_parent_item) {

    //        auto r = _parent_item->delete_permanent(this);
    //        assert(!r);

    //        //        for(auto it : _parent_item->_child_linkers) { // for(int row = 0; row < _child_items.size(); ++row) {
    //        //            //        auto it = _child_items.at(row);

    //        //            if(it->host()->id() == _host->id()) { //_child_items.removeAt(position);    // _child_items.takeAt(position).reset(); // delete _child_items.takeAt(position);
    //        //                _parent_item->_child_linkers.removeOne(it);
    //        //                it->host()->page_break();

    //        //                if(it->host() != _host)_host->merge(it->host());

    //        //                it.reset();;   //                it->parent(nullptr);



    //        //                //                else
    //        //                //                    result = _child_item;
    //        //            }
    //        //        }
    //    }

    //    //    _parent_item->_record_linker = nullptr;
    _host_parent = nullptr;

    //    self_remove_from_parent_as_empty();

    //    return result;
}




Linker::Linker(boost::intrusive_ptr<TreeItem>  host_parent_item, boost::intrusive_ptr<TreeItem> host_item) // , int pos, int mode
    : boost::intrusive_ref_counter<Linker, boost::thread_safe_counter>()
    , _host_parent([ & ]()->boost::intrusive_ptr<TreeItem> {assert(host_parent_item != host_item); return host_parent_item;}())         // _new_parent
, _host(host_item
        //    [&]()
        //{
        //    //    _host = nullptr;
        //    //    auto linker_ = new_host->up_linker();

        //    //    if(linker_) {
        //    //        auto _old_host_parent = linker_->host_parent();

        //    //        if(_old_host_parent
        //    //           && _old_host_parent != _parent_item
        //    //           && _old_host_parent->contains_direct(new_host)
        //    //           && _old_host_parent->id() != new_host->_field_data.value("id")
        //    //          ) {
        //    //            _old_host_parent->remove(new_host->up_linker());
        //    //        }
        //    //    }

        //    return new_host;
        //}()
       )
, child_move_unique(
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
[&](boost::intrusive_ptr<TreeItem> _parent, int pos = 0, int mode = add_new_record_before)->boost::intrusive_ptr<Linker> { //        , boost::intrusive_ptr<TreeItem>   _host   // child_insert? does not set parent pointer?

    boost::intrusive_ptr<Linker> result(nullptr);

    //        boost::intrusive_ptr<TreeItem> _child_item(this);

    //            assert(_self != _parent);

    if(_host != _parent)
    {
        int actually_insert_position = -1;
        int result_should_be_position = pos;
        int count = 0;
        int found = 0;

        if(_parent) {
            for(auto il : _parent->_child_linkers) { //= _parent->_child_linkers.begin(); it != _parent->_child_linkers.end(); it++) {
                if(il == this) {
                    found++;

                    if(found == 1) {
                        result = il;

                        if(il->_host_parent != _parent)il->_host_parent = _parent;

                        actually_insert_position = count;
                    } else {
                        _parent->_child_linkers.removeOne(il);
                    }
                }

                count++;
            }

            //        if(0 == found) {
            // Запись добавляется в таблицу конечных записей



            // get shadow_branch
            //    TreeModelKnow *shadow_branch = globalparameters.tree_screen()->_shadow_branch;  // static_cast<TreeModelKnow *>(find_object<TreeViewKnow>(knowtreeview_singleton_name)->model());

            if(0 == found) {              // !item_direct(_source_item)
                //       && !shadow_branch->is_record_id_exists(item->field("id"))

                //                // deprecated by _parent_target->remove
                //                auto _origin_parent = _source_item->parent();

                //                if(_origin_parent && _origin_parent != _parent_target) {
                //                    //                    if(_parent != this) {
                //                    _origin_parent->remove(_source_item);

                //                    //                    while(_origin_parent->remove_self_as_empty()) {
                //                    //                        _origin_parent = _origin_parent->parent();

                //                    //                        if(!_origin_parent)break;
                //                    //                    }

                //                    //                    }
                //                }

                // !!!, this make item move to new branch, remember to remove orignal one
                //                    _child_item->_parent_item = _parent_target; // _source_item->parent(boost::intrusive_ptr<TreeItem>(_parent_target));    //_item->parent(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)));








                //                    if(_host->up_linker() != this)_host->up_linker(this);









                //        if(_tree_item) qDebug() << "RecordTable::insert_new_record() : Insert new record to branch " << _tree_item->all_fields();



                // Выясняется, есть ли в дереве запись с указанным ID
                // Если есть, то генерируются новые ID для записи и новая директория хранения
                // Если нет, то это значит что запись была вырезана, но хранится в буфере,
                // и ее желательно вставить с прежним ID и прежним именем директории
                // It turns out, is there a record of a tree with specified ID
                // If there is, then the generated ID for the new record and the new storage directory
                // If not, then it means that the recording was cut out, but stored in a buffer
                // And it is desirable to stick with the same ID and the same name directory
                //        KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>(knowtreeview_singleton_name)->model());

                if(0 == _host->field("id").length()) { //           || dataModel->isRecordIdExists(record->getField("id"))  // ? Is this a correct policy? can we only create a pure view?
                    // Создается новая запись (ID был пустой) или
                    // Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)
                    // Create a new record (ID was empty) or
                    // Record with ID in a tree there, so stand the new ID and a new storage directory (not to overwrite existing)

                    QString id = get_unical_id();

                    // Store directory entries and files    // Директория хранения записи и файл
                    if(_host->field("dir") == "")_host->field("dir", id); // get_unical_id()

                    if(_host->field("file") == "")_host->field("file", "text.html");

                    // Unique ID of XML records             // Уникальный идентификатор XML записи
                    //            QString id = get_unical_id();
                    _host->field("id", id);
                } else {
                    if(_host->field("dir") == "")_host->field("dir", _host->field("id")); // get_unical_id()

                    if(_host->field("file") == "")_host->field("file", "text.html");
                }

                // The method must take a full-fledged object record    // Мотод должен принять полновесный объект записи
                if(_host->is_lite()) {
                    _host->to_fat();
                    //            critical_error("ItemsFlat::insert_new_item() can't insert lite record");
                }

                // В список переданных полей добавляются вычислимые в данном месте поля
                if(_host->field("ctime") == "") {
                    // Время создания данной записи
                    QDateTime ctime_dt = QDateTime::currentDateTime();
                    QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
                    _host->field("ctime", ctime);

                }

                // Выясняется в какой ветке вставляется запись - в зашифрованной или нет
                bool is_crypt = false;

                //        if(_tree_item) {
                //                if(_is_crypt) { //
                if(_parent->field("crypt") == "1") {
                    if(globalparameters.crypt_key().length() > 0)
                        is_crypt = true;
                    else
                        critical_error("ItemsFlat::insert_new_item() : Can not insert data to crypt branch. Password not setted.");
                }

                //        }

                // Запись полновесных данных с учетом шифрации
                if(is_crypt && _host->field("crypt") != "1") {       // В зашифрованную ветку незашифрованную запись
                    _host->to_encrypt_and_save_fat();
                } else if(!is_crypt && _host->field("crypt") == "1") { // В незашифрованную ветку зашифрованную запись
                    _host->to_decrypt_and_save_fat();
                } else {
                    _host->push_fat_attributes();
                }

                // Record switch to easy mode to be added to the final table of records ?***    // Запись переключается в легкий режим чтобы быть добавленной в таблицу конечных записей
                _host->to_lite();

                //        //        // Запись добавляется в таблицу конечных записей
                //        //        int insertPos = -1;

                //        item->is_registered_to_record_controller_and_tabmanager(true);

                if(mode == add_new_record_to_end) {         // В конец списка           // mode == 0
                    _parent->_child_linkers << this;        // _self;
                    result_should_be_position = _parent->_child_linkers.size() - 1; // parent may be empty before
                } else if(mode == add_new_record_before) {  // Перед указанной позицией // mode == 1
                    _parent->_child_linkers.insert(pos, this);
                    result_should_be_position = pos < 0 ? 0 : pos;
                } else if(mode == add_new_record_after) {   // После указанной позиции  // mode == 2
                    _parent->_child_linkers.insert(pos + 1, this);
                    auto last = _parent->_child_linkers.size() - 1;
                    result_should_be_position = pos >= last ? last : pos + 1;  // parent may be empty before
                }

                if(this->_host_parent != _parent)this->_host_parent = _parent;

                //                    insert_position =  _parent->sibling_order(_self);  //_child_item->record_linker()->sibling_order();   // ItemsFlat::
                //                    _parent_target->sibling_order(_source_item);
                //                    qDebug() << "ItemsFlat::insert_new_item() : New record pos" << QString::number(insert_position);

                // Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода

            }

            //                else {
            //                    insert_position = _parent->sibling_order(_self);  // _child_item->sibling_order();    // ItemsFlat::
            //                    //                    _parent_target->sibling_order(_source_item);
            //                }

            actually_insert_position = _parent->sibling_order([&](boost::intrusive_ptr<const Linker> il) {return il == this && il == _host->linker() && il->host() == _host;});

            //            int insert_position = _host_parent->sibling_order([&](boost::intrusive_ptr<const Linker> il) {return il == _host->linker() && il->host() == _host;});


            //            if(mode == add_new_record_after)result_should_be_position = _host_parent->count_direct() == 1 ? pos : pos + 1; // parent may be empty before
            //            else if(mode == add_new_record_to_end)result_should_be_position = _host_parent->count_direct() - 1; // parent may be empty before

            assert(result_should_be_position == actually_insert_position);

            qDebug() << "ItemsFlat::insert_new_item() : New record pos" << QString::number(actually_insert_position);
            assert(_parent->_child_linkers[actually_insert_position] == this);

            auto it = _parent->_child_linkers.contains(this);

            if(it) {
                result = this;


                auto item = _host_parent ? _host_parent->contains_direct(std::forward < const boost::intrusive_ptr<const Linker> > (this)) : nullptr;
                auto linker_ = item ? item->linker() : nullptr;


                auto linker = std::get<3>(_status)();
                assert(this == std::get<3>(_status)());
            } else {
                assert(false);
                //                assert(std::get<3>(_status)());
            }

            //        }
            assert(std::get<3>(_status)());
        }
    }

    //    else {
    //        result = boost::intrusive_ptr<TreeItem>(this);
    //    }

    return result;    // insert_position;

})
, integrity(

[&]()->bool {
    bool result = true;

    if(!std::get<0>(_status)())result = false;

    if(!std::get<1>(_status)()) {if(_host_parent)result = false;}

    if(// !std::get<2>(_status)()   // change rvalue to lvalue!!!
        //        host()->up_linker(); // _host is under construction at this time
        std::get<2>(_status)() != nullptr)
    {
        _host->linker(std::forward<boost::intrusive_ptr<Linker>>(this));   // change rvalue to lvalue!!!

        if(!_host->linker()) {  // change rvalue to lvalue!!!
            result = false;
            assert(result);
        }
    }

    if(!std::get<3>(_status)())
    {
        this->child_move_unique(_host_parent, 0, add_new_record_before);

        if(_host_parent) {
            if(!_host_parent->contains_direct(std::forward <const boost::intrusive_ptr<const Linker>> (this))) {
                result = false;
                assert(result);
            }
        }
    }

    if(!std::get<4>(_status)())
    {
        assert(result);
        result = false;
    }

    if(!std::get<5>(_status)())
    {
        assert(result);
        result = false;
    }

    return result;
}
)
, _status(  // state_impl()
    //    [&]()
    //{

    //    //    status_type status;

    //    std::get<0>(_status) = [&]() {return host();};  //

    //    std::get<1>(_status) = [&]() {return host_parent();};  //

    //    std::get<2>(_status) = [&]() {return _host->up_linker();};  // std::forward < const boost::intrusive_ptr<Linker> > (this);    // you cannot forward something more than once?

    //    std::get<3>(_status) = [&]() {
    //        auto it = _host_parent ? _host_parent->contains_direct(std::forward < const boost::intrusive_ptr<Linker> > (this)) : nullptr;
    //        return it ? it->up_linker() : nullptr;
    //    }; //

    //    std::get<4>(_status) = [&]() {return _host->up_linker() == this;};  //

    //    std::get<5>(_status) = [&]() {return std::get<3>(_status)() == this;}; //

    //    std::get<6>(_status) = [&](boost::intrusive_ptr<TreeItem> host_) {return host_ == _host;};  //

    //    std::get<7>(_status) = [&](boost::intrusive_ptr<TreeItem> host_parent_) {return host_parent_ == _host_parent;};  //

    //    //    return std::move(status);
    //    return _status;
    //}()

)
{

    [&]() {

        //    status_type status;

        std::get<0>(_status) = [&]() {return host();};  //

        std::get<1>(_status) = [&]() {return host_parent();};  //

        std::get<2>(_status) = [&]() {return _host->linker();};  // std::forward < const boost::intrusive_ptr<Linker> > (this);    // you cannot forward something more than once?

        std::get<3>(_status) = [&]() {
            auto it = _host_parent ? _host_parent->contains_direct(std::forward <const boost::intrusive_ptr<const Linker>> (this)) : nullptr;
            return it ? it->linker() : nullptr;
        }; //

        std::get<4>(_status) = [&]() {return _host->linker() == this;};  //

        std::get<5>(_status) = [&]() {return std::get<3>(_status)() == this;}; //

        std::get<6>(_status) = [&](boost::intrusive_ptr<const TreeItem> host_) {return host_ == _host;};  //

        std::get<7>(_status) = [&](boost::intrusive_ptr<const TreeItem> host_parent_) {return host_parent_ == _host_parent;};  //

        //    return std::move(status);
        //        return _status;
    }();

    //    //    auto internal_contain_check = [](boost::intrusive_ptr<TreeItem> parent_, linker * const this_)->linker* {
    //    //        linker *result = nullptr;

    //    //        for(auto il : parent_->_child_linkers)
    //    //        {
    //    //            //
    //    //        }

    //    //        return result;
    //    //    };



    //    //    [ & ]()->boost::intrusive_ptr<TreeItem> {
    //    //        this->_parent_item = _parent;
    //    //        this->_child_item = _child_item;


    //    boost::intrusive_ptr<Linker> _result(nullptr);

    //    if(_host_parent) {
    //        if(!_host_parent->contains_direct(std::forward < const boost::intrusive_ptr<Linker> > (this)) // _host->up_linker()   // _host is under contruction at this time
    //           && _host_parent->id() != _host->_field_data.value("id")) {







    //            //        this->child_move_unique =



    //            //    if(_parent_item && _parent_item != _new_parent) {
    //            //        _parent_item->remove(_child_item);
    //            //    }




    //            //    if(_parent_item && _parent_item != _parent) {
    //            //        _parent_item->remove(_child_item);
    //            //    }

    //            //        auto _child_item_parent = _child_item->parent();  // form ItemsFlat, incomplete

    //            //        if(_child_item_parent && _child_item_parent != _parent_item) {
    //            //            _child_item_parent->remove(_child_item);
    //            //        }


    //            //        if(_parent) {

    //            //        assert(_parent_item->id() != _child_item->_field_data.value("id"));   //        assert(_parent_item->name() != _field_data.value("name"));

    //            // _parent_item->add_child(boost::intrusive_ptr<TreeItem>(this));



    //            //            //        }

    //            //            //    _parent_item = _new_parent;

    //            //            //            if(_parent) {    //  && _parent_item != it
    //            //            //        _parent_item = it;
    //            //            bool found = false;

    //            //            for(auto i : _parent_item->_child_items) {
    //            //                if(_child_item->id() == i->id()) {found = true; break;}
    //            //            }

    //            //            if(!found) {
    //            //                //            _result = _parent_item->child_move_unique(this
    //            //                //                                                      , _pos  // _parent_item->count_direct() > 0 ? _parent_item->count_direct() - 1 : 0
    //            //                //                                                      , _mode
    //            //                //                                                     ); // _parent_item->_child_items << boost::intrusive_ptr<TreeItem>(this);

    //            _result = child_move_unique(_host_parent, pos, mode);    // , _host

    //            // _parent_item->count_direct() > 0 ? _parent_item->count_direct() - 1 : 0
    //            // _parent_item->_child_items << boost::intrusive_ptr<TreeItem>(this);
    //            //            }

    //            if(_host_parent->_field_data["crypt"] == "1") {
    //                _host->to_encrypt();
    //            } else {
    //                _host->to_decrypt();
    //            }

    //            //        }


    //            //        }

    //            //        if(_parent_item && _parent_item->up_linker() != this)_parent_item->up_linker(this); // deprecated ItemsFlat::_child_items move to ItemsFlat::_child_linkers

    //        }
    //    }

    //    //    auto is_integratied = integrity();
    //    //    assert(is_integratied);

    //    //    //    //    return // _parent_item =
    //    //    //    //        _parent;
    //    //    //    //    }();

    //    //    //    //    [&]()->boost::intrusive_ptr<TreeItem> {
    //    //    //    _child_item->record_linker(this);
    //    //    //    //        return _child_item;
    //    //    //    //    }();  //

    //    //    //    //    return _result;
}



int Linker::sibling_order() const
{
    int result = -1;

    if(_host_parent)
        result = _host_parent->_child_linkers.indexOf(_host->linker()); // boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()

    return result;
}

int Linker::move_up(void)
{
    int row = 0;
    // Выясняется номер данного элемента в списке родителя
    int num = sibling_order();

    if(num > 0 && num < (_host_parent->count_direct())) {   // if(num == 0)return false;    // Если двигать вверх некуда, ничего делать ненужно
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
    if(num > 0 && num < (_host_parent->count_direct())) { //if(num >= (_parent_item->count_direct()))return false;

        row = 1;
        // Элемент перемещается вниз по списку
        (_host_parent->_child_linkers).swap(num, num + row);
    }

    return row;
}


boost::intrusive_ptr<TreeItem> Linker::host_parent()const {return _host_parent;}
boost::intrusive_ptr<TreeItem> Linker::host()const {return _host;}


void Linker::host_parent(boost::intrusive_ptr<TreeItem> &&p) {_host_parent = p;}
void Linker::host(boost::intrusive_ptr<TreeItem> &&h) {_host = h;}

boost::intrusive_ptr<Linker> Linker::parent(boost::intrusive_ptr<TreeItem> parent_item, const int pos, const int mode)
{
    assert(_host);
    boost::intrusive_ptr<Linker> result(nullptr);

    if(parent_item) {
        if( // parent_item != _host_parent && !parent_item->contains_direct(std::forward < const boost::intrusive_ptr<linker> > (this)) // _host->up_linker()
            // ||
            !integrity_external(this->_host, parent_item)
            && parent_item->id() != _host->_field_data.value("id")) {

            assert(_host->linker() == this);
            assert(_host->linker()->host_parent() == this->_host_parent);
            //        auto _parent_item = _host->up_linker()->parent_item();

            if(_host_parent) {
                if(_host_parent != parent_item
                   && _host_parent->contains_direct(std::forward < const boost::intrusive_ptr<const Linker> > (this)) // _host->up_linker()
                   && _host_parent->id() != _host->_field_data.value("id")
                  ) {
                    //            _parent_item->delete_permanent(_host->up_linker());    // leading boost::intrusive_ptr<linker> reseted!!!, so this function may be an illegal operation?
                    _host_parent = parent_item;
                }
            }

            if(parent_item) {
                result = child_move_unique(parent_item, pos, mode);  // , _host


                if( // new_host_parent &&
                    parent_item->_field_data["crypt"] == "1") {
                    _host->to_encrypt();
                } else {
                    _host->to_decrypt();
                }
            }

            //        _host_parent = new_host_parent;
            assert(integrity());
            assert(integrity_external(this->_host, _host_parent));

            //            int actually_insert_position = _host_parent->sibling_order([&](boost::intrusive_ptr<const Linker> il) {return il == _host->linker() && il->host() == _host;});
            //            int result_should_be_position = pos;

            //            if(mode == add_new_record_after)result_should_be_position = _host_parent->count_direct() == 1 ? pos : pos + 1; // parent may be empty before
            //            else if(mode == add_new_record_to_end)result_should_be_position = _host_parent->count_direct() - 1; // parent may be empty before

            //            assert(result_should_be_position == actually_insert_position);
        }
    }

    return result;
}

void Linker::remove()
{
    //    if(_host_parent->up_linker())_host_parent->up_linker(nullptr);    // pretty wrong semantic!
    _host->dangle();

    //    if(_host->up_linker()) {
    //        // boost::intrusive_ptr<TreeItem> _parent = _host->up_linker()->_host_parent;
    //        // _parent = nullptr;
    //        _host->up_linker().reset();
    //    }
}



bool Linker::integrity_internal()const
{
    return std::get<0>(_status)() && std::get<1>(_status)() && std::get<2>(_status)() && std::get<3>(_status)() && std::get<4>(_status)() && std::get<5>(_status)();
}

bool Linker::integrity_external(boost::intrusive_ptr<const TreeItem> host_, boost::intrusive_ptr<const TreeItem> host_parent_)const
{
    return std::get<0>(_status)() && std::get<1>(_status)() && std::get<2>(_status)() && std::get<3>(_status)() && std::get<4>(_status)() && std::get<5>(_status)() && std::get<6>(_status)(host_) && std::get<7>(_status)(host_parent_);
}



//Linker::status_type Linker::state_impl()
//{
//    status_type status;

//    std::get<0>(status) = [&]() {return host();};  //

//    std::get<1>(status) = [&]() {return host_parent();};  //

//    std::get<2>(status) = [&]() {return _host->up_linker();};  // std::forward < const boost::intrusive_ptr<Linker> > (this);    // you cannot forward something more than once?

//    std::get<3>(status) = [&]() {return _host_parent ? _host_parent->contains_direct(std::forward < const boost::intrusive_ptr<Linker> > (this))->up_linker() : nullptr;}; //

//    std::get<4>(status) = [&]() {return _host->up_linker() == this;};  //

//    std::get<5>(status) = [&]() {return std::get<3>(status)() == this;}; //

//    std::get<6>(status) = [&](boost::intrusive_ptr<TreeItem> host_) {return host_ == _host;};  //

//    std::get<7>(status) = [&](boost::intrusive_ptr<TreeItem> host_parent_) {return host_parent_ == _host_parent;};  //

//    return std::move(status);
//}   // is_closure

//bool Linker::integrity()
//{
//    bool result = true;

//    if(!std::get<0>(_status)())result = false;

//    if(!std::get<1>(_status)()) {if(_host_parent)result = false;}

//    if(// !std::get<2>(_status)()   // change rvalue to lvalue!!!
//        //        host()->up_linker(); // _host is under construction at this time
//        std::get<2>(_status)() != nullptr) {
//        _host->up_linker(std::forward<boost::intrusive_ptr<linker>>(this));   // change rvalue to lvalue!!!

//        if(!_host->up_linker())   // change rvalue to lvalue!!!
//            result = false;
//    }

//    if(!std::get<3>(_status)()) {
//        this->child_move_unique(_host_parent, 0, ADD_NEW_RECORD_BEFORE);

//        if(_host_parent) {
//            if(!_host_parent->contains_direct(std::forward < const boost::intrusive_ptr<linker> > (this)))
//                result = false;
//        }
//    }

//    return result;
//};
