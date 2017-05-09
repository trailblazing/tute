

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QDrag>
#include <QHeaderView>
#include <QMimeData>
#include <QObject>
#include <QScrollBar>
#include <QStyleOptionToolBar>
#include <QVector>

//#include "models/tree/tree_item.dec"

#include "controllers/record_table/record_controller.h"
#include "libraries/clipboard_records.h"
#include "libraries/fixed_parameters.h"
#include "libraries/flat_control.h"
#include "libraries/global_parameters.h"
#include "libraries/qtm/blogger.h"
#include "libraries/window_switcher.h"
#include "main.h"
#include "models/app_config/app_config.h"
#include "models/record_table/items_flat.h"
#include "models/record_table/linker.hxx"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "models/record_table/record_model_proxy.h"
#include "models/tree/tree_index.hxx"
#include "models/tree/tree_know_model.h"
#include "record_view.h"
#include "views/main_window/main_window.h"
#include "views/main_window/main_window.h"
#include "views/record/editor_wrap.h"
#include "views/record_table/record_screen.h"
#include "views/record_table/vertical_scrollarea.h"
#include "views/record_table/vertical_scrollarea.h"
#include "views/tree/tree_screen.h"
#include "views/tree/tree_view.h"

extern std::shared_ptr<gl_para> gl_paras;
extern FixedParameters fixedparameters;
extern std::shared_ptr<AppConfig> appconfig;
extern const char* record_view_multi_instance_name;
extern enum QItemSelectionModel::SelectionFlag current_tree_selection_mode;
extern enum QItemSelectionModel::SelectionFlag current_tree_current_index_mode;

enum QItemSelectionModel::SelectionFlag current_record_selection_mode =
    QItemSelectionModel::SelectionFlag::ClearAndSelect; // Select//    Current       // ClearAndSelect //  |
							// QItemSelectionModel::SelectionFlag::Rows
enum QItemSelectionModel::SelectionFlag current_record_current_index_mode =
    QItemSelectionModel::SelectionFlag::SelectCurrent; // Select   //
						       // SelectCurrent

#ifdef USE_STAR_RATING

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(FlatToolButtonRating)
#endif

FlatToolButtonRating::FlatToolButtonRating(QWidget* parent)
    : FlatToolButton(parent)
{
	// setMouseTracking(true);
	setAutoFillBackground(true);
}

void FlatToolButtonRating::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	_star_rating.paint(&painter, e->rect(), this->palette(), StarRating::Editable);

	QStyleOptionButton opt;
	opt.state = QStyle::State_Active | QStyle::State_Enabled;
	opt.rect = QRect(50, 25, 100, 50);
	// QPainter painter(this);
	this->style()->drawControl(QStyle::CE_PushButton, &opt, &painter,
	    nullptr); // button.data()
}

void FlatToolButtonRating::mouseMoveEvent(QMouseEvent* event)
{
	(void)event;
	// int star = starAtPosition(event->x());
	// if(star != myStarRating.starCount() && star != - 1){
	// myStarRating.setStarCount(star);
	// update();
	// }
}

// ! [2]

// ! [3]
void FlatToolButtonRating::mouseReleaseEvent(QMouseEvent* /* event */)
{
	emit editingFinished();
}

const int _painting_scale_factor = 20;

// ! [0]
StarRating::StarRating(int starCount, int maxStarCount)
{
	_star_count = starCount;
	_max_star_count = maxStarCount;

	_star_polygon << QPointF(1.0, 0.5);
	for (int i = 1; i < 5; ++i)
		_star_polygon << QPointF(0.5 + 0.5 * std::cos(0.8 * i * 3.14), 0.5 + 0.5 * std::sin(0.8 * i * 3.14));
	_diamond_polygon << QPointF(0.4, 0.5) << QPointF(0.5, 0.4)
			 << QPointF(0.6, 0.5) << QPointF(0.5, 0.6)
			 << QPointF(0.4, 0.5);
}

// ! [0]

// ! [1]
QSize StarRating::sizeHint() const
{
	return _painting_scale_factor * QSize(_max_star_count, 1);
}

// ! [1]

// ! [2]
void StarRating::paint(QPainter* painter, const QRect& rect, const QPalette& palette, EditMode mode) const
{
	painter->save();

	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->setPen(Qt::NoPen);
	if (mode == Editable)
		painter->setBrush(palette.highlight());
	else
		painter->setBrush(palette.foreground());
	int yOffset = (rect.height() - _painting_scale_factor) / 2;
	painter->translate(rect.x(), rect.y() + yOffset);
	painter->scale(_painting_scale_factor, _painting_scale_factor);
	for (int i = 0; i < _max_star_count; ++i) {
		if (i < _star_count)
			painter->drawPolygon(_star_polygon, Qt::WindingFill);
		else if (mode == Editable)
			painter->drawPolygon(_diamond_polygon, Qt::WindingFill);
		painter->translate(1.0, 0.0);
	}
	painter->restore();
}

#endif

// ! [2]
// !
// !
#ifdef USE_BUTTON_COLUMN

#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include <QStylePainter>
#include <QTableView>

ButtonColumnDelegate::ButtonColumnDelegate(rv_t* view) // (QObject *parent)
    : QStyledItemDelegate(view),
      _view(view)
{
	// if(rv_t *tableView = qobject_cast<rv_t *>(view)){
	// _view	= tableView;
	_button = new QPushButton("...", _view);
	_button->hide();
	_view->setMouseTracking(true);
	connect(_view, &rv_t::entered, this, &ButtonColumnDelegate::cellEntered);
	_is_one_cell_in_edit_mode = false;
	// }
}

ButtonColumnDelegate::~ButtonColumnDelegate()
{
}

// createEditor
QWidget* ButtonColumnDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (index.model()
		->headerData(index.column(), Qt::Horizontal, Qt::UserRole)
		.toInt() == 1) {
		QPushButton* btn = new QPushButton(parent);
		btn->setText(index.data().toString());

		return btn;
	} else
		return QStyledItemDelegate::createEditor(parent, option, index);
}

// setEditorData
void ButtonColumnDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	if (index.model()
		->headerData(index.column(), Qt::Horizontal, Qt::UserRole)
		.toInt() == 1) {
		QPushButton* btn = qobject_cast<QPushButton*>(editor);
		btn->setProperty("data_value", index.data());
	} else
		QStyledItemDelegate::setEditorData(editor, index);
}

// setModelData
void ButtonColumnDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	if (index.model()
		->headerData(index.column(), Qt::Horizontal, Qt::UserRole)
		.toInt() == 1) {
		QPushButton* btn = qobject_cast<QPushButton*>(editor);
		model->setData(index, btn->property("data_value"));
	} else
		QStyledItemDelegate::setModelData(editor, model, index);
}

// paint
void ButtonColumnDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (index.model()
		->headerData(index.column(), Qt::Horizontal, Qt::UserRole)
		.toInt() == 1) {
		_button->setGeometry(option.rect);
		_button->setText(index.data().toString());
		if (option.state == QStyle::State_Selected)
			painter->fillRect(option.rect, option.palette.highlight());
		QPixmap map = QPixmap::grabWidget(_button);
		painter->drawPixmap(option.rect.x(), option.rect.y(), map);
	} else
		QStyledItemDelegate::paint(painter, option, index);
}

// updateGeometry
void ButtonColumnDelegate::updateEditorGeometry(
    QWidget* editor, const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
	(void)index;
	editor->setGeometry(option.rect);
}

// cellEntered
void ButtonColumnDelegate::cellEntered(const QModelIndex& index)
{
	if (index.model()->headerData(index.column(), Qt::Horizontal, Qt::UserRole) ==
	    1) {
		if (_is_one_cell_in_edit_mode)
			_view->closePersistentEditor(_current_edited_cell_index);
		_view->openPersistentEditor(index);
		_is_one_cell_in_edit_mode = true;
		_current_edited_cell_index = index;
	} else {
		if (_is_one_cell_in_edit_mode) {
			_is_one_cell_in_edit_mode = false;
			_view->closePersistentEditor(_current_edited_cell_index);
		}
	}
}

#endif

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(ViewDelegation)
#endif

ViewDelegation::ViewDelegation(rv_t* view)
    : QStyledItemDelegate(view)
    , _view(view)
    , _scroll_bar_width(10)
    , _rating_width(30)
    , _x_offset(_rating_width)
{
	auto column_count = appconfig->record_table_show_fields().size();
	for (int i = 0; i < column_count; i++) {
		auto rctrl = _view->record_ctrl();
		if (rctrl) {
			auto header_title = rctrl->source_model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString(); // DisplayRole?UserRole
			auto rating_field_description = fixedparameters.record_field_description(
			    QStringList() << boost::mpl::c_str<
				rating_key>::value)[boost::mpl::c_str<rating_key>::value];
			if (header_title == rating_field_description)
				for (int j = 0; j < rctrl->source_model()->rowCount(); j++)
					_view->openPersistentEditor(rctrl->source_model()->index(j, i, QModelIndex()));
		}
	}
}

// template <typename field_type>
// bool RecordView::is_rating_column(QModelIndex index){
// auto header_title =
// _record_controller->source_model()->headerData(index.column(),
// Qt::Horizontal, Qt::DisplayRole).toString(); // DisplayRole?UserRole
// auto rating_field_description =
// fixedparameters.record_field_description(QStringList() <<
// boost::mpl::c_str<field_type>::value)[boost::mpl::c_str < field_type >
// ::value];
// return header_title == rating_field_description;
// }

void ViewDelegation::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// QStyleOptionViewItemV4 opt = setOptions(index, option);

	// prepare
	painter->save();

	auto _record_ctrl = _view->record_ctrl();
	if (_record_ctrl) {
		auto it = _record_ctrl->source_model()->item(_record_ctrl->index<pos_source>(_record_ctrl->index<pos_proxy>(index_proxy(index)))); // pos_source(pos_proxy(index.row()))
		auto header_title = _record_ctrl->source_model()->headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();          // DisplayRole?UserRole

		auto rating_field_description = fixedparameters.record_field_description(QStringList() << boost::mpl::c_str<rating_key>::value)[boost::mpl::c_str<rating_key>::value];
		auto pin_field_description = fixedparameters.record_field_description(QStringList() << boost::mpl::c_str<pin_key>::value)[boost::mpl::c_str<pin_key>::value];
		auto title_field_description = fixedparameters.record_field_description(QStringList() << boost::mpl::c_str<name_key>::value)[boost::mpl::c_str<name_key>::value];
		if (it && header_title == rating_field_description) { // index.column() == 0
#ifdef USE_STAR_RATING
			StarRating* starRating =
			    it->star_rating(); // qvariant_cast<StarRating>(index.data());
			if (option.state & QStyle::State_Selected)
				painter->fillRect(option.rect, option.palette.highlight());
			starRating->paint(painter, option.rect, option.palette, StarRating::ReadOnly);

#elif defined USE_TEXT_AS_BUTTON
			////        StarRating *star_rating = it->star_rating(); // =
			///qvariant_cast<StarRating>(index.data());
			// if(option.state & QStyle::State_Selected)painter->fillRect(option.rect,
			// option.palette.highlight());
			////        star_rating->paint(painter, option.rect, option.palette,
			///StarRating::ReadOnly);

			QStyleOptionButton optionV4;
			optionV4.state |= QStyle::State_Enabled;
			if (option.state & QStyle::State_Selected)
				painter->fillRect(option.rect, option.palette.highlight());
			optionV4.features = optionV4.features | QStyleOptionButton::ButtonFeature::Flat | QStyleOptionButton::ButtonFeature::CommandLinkButton;
			optionV4.rect = option.rect.adjusted(0, 0, 0, 0); // adjusted(1, 1, - 1, - 1);	// QRect(50, 25, 100, 50);//
			////        auto title =
			///_view->record_ctrl()->source_model()->item(PosSource(PosProxy(index.row())))->field<name_type>();
			optionV4.text = QChar(0x274C);                                             // for ❌//QChar(0x274E);	// for "❎"; "X";
												   // // title;         // trUtf8("Button
												   // text");
			_view->style()->drawControl(QStyle::CE_PushButton, &optionV4, painter, 0); // opt.paint(painter, option.rect,
												   // option.palette,
												   // QStyleOptionButton::ReadOnly);
												   // it->star_rating()->paint(painter, option.rect, option.palette,
												   // StarRating::Editable);

// QApplication::style()->drawControl(QStyle::CE_PushButton, &option, painter);

//// get the data and the rectangles
// const QPixmap	&pixmap		=
// qvariant_cast<QPixmap>(index.data(Qt::UserRole + 1));
// QRect		decorationRect	= QRect(option.rect.topLeft(), QSize(80,
// 80));
// const QString	&text		= index.data(Qt::UserRole).toString();
// QFontMetrics	fm(painter->font());
// QRect		displayRect	= QRect(decorationRect.topRight() + QPoint(20,
// 30), QSize(fm.width(text), fm.height()));
// const QPixmap	&pixmapSmall	=
// qvariant_cast<QPixmap>(index.data(Qt::UserRole + 2));
// QRect		smallIconRect	= QRect(option.rect.topRight() - QPoint(100,
// - 20), QSize(20, 20));
//// drawBackground(painter, option, index);
// painter->drawPixmap(decorationRect, pixmap);
// painter->drawText(displayRect, text);
// painter->drawPixmap(smallIconRect, pixmapSmall);
////	if(index.data().canConvert<QStyleOptionButton>()){
////	    QStyleOptionButton opt;
// opt.state |= QStyle::State_Enabled;
// if(option.state & QStyle::State_Selected) painter->fillRect(option.rect,
// option.palette.highlight());
// opt.rect = option.rect.adjusted(1, 1, - 1, - 1);//(1, 1, - 10, - 10);
// auto title =
// _view->record_ctrl()->source_model()->item(pos_source(pos_proxy(index.row())))->field<name_type>();
// opt.text = title;	// trUtf8("Button text");
// QApplication::style()->drawControl(QStyle::CE_PushButton, &opt, painter,
// 0);// opt.paint(painter, option.rect, option.palette,
// QStyleOptionButton::ReadOnly);
////	}else QStyledItemDelegate::paint(painter, option, index);

#else

			// #ifdef USE_BUTTON
			/// https://qtadventures.wordpress.com/2012/02/04/adding-button-to-qviewtable/
			/// https://stackoverflow.com/questions/11777637/adding-button-to-qtableview
			QStyleOptionButton button;
			QRect r = option.rect; // getting the rect of the cell
			int x, y, w, h;

			x = r.left() + r.width() - _x_offset;  // the X coordinate
			y = r.top() + 1;                       // the Y coordinate
			w = _rating_width - _scroll_bar_width; // button width
			h = 16;                                // button height
			button.rect = QRect(x, y, w, h);
			button.text =
			    QChar(0x274E); // for "❎";	// √ ✅	// "><";	// "=^.^=";
			button.state = QStyle::State_Enabled;

			QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
#endif
		} else if (it && header_title == pin_field_description && it->field<pin_key>() != pin_value(bool_from_check_state[Qt::Unchecked])) {
#ifdef USE_STAR_RATING

#elif defined USE_TEXT_AS_BUTTON
			////        StarRating *star_rating = it->star_rating(); // =
			///qvariant_cast<StarRating>(index.data());
			// if(option.state & QStyle::State_Selected)painter->fillRect(option.rect,
			// option.palette.highlight());
			////        star_rating->paint(painter, option.rect, option.palette,
			///StarRating::ReadOnly);

			QStyleOptionButton optionV4;
			optionV4.state |= QStyle::State_Enabled;
			if (option.state & QStyle::State_Selected)
				painter->fillRect(option.rect, option.palette.highlight());
			optionV4.features = optionV4.features | QStyleOptionButton::ButtonFeature::Flat | QStyleOptionButton::ButtonFeature::CommandLinkButton;
			optionV4.rect = option.rect.adjusted(0, 0, 0, 0); // adjusted(1, 1, - 1, -
			// 1);		//
			// QRect(50, 25, 100,
			// 50);//
			////        auto title =
			///_view->record_ctrl()->source_model()->item(PosSource(PosProxy(index.row())))->field<name_type>();
			optionV4.text = QChar(0x221A);                                             // for √ // title;// trUtf8("Button text");
			_view->style()->drawControl(QStyle::CE_PushButton, &optionV4, painter, 0); // opt.paint(painter, option.rect,
												   // option.palette,
												   // QStyleOptionButton::ReadOnly);

#else

#endif
		}
		// else if(it && header_title == title_field_description && it ==
		// _view->current_item()){
		// #ifdef USE_STAR_RATING
		// #elif defined USE_TEXT_AS_BUTTON
		//////        StarRating *star_rating = it->star_rating(); // =
		///qvariant_cast<StarRating>(index.data());
		////	if(option.state & QStyle::State_Selected)painter->fillRect(option.rect,
		///option.palette.highlight());
		//////        star_rating->paint(painter, option.rect, option.palette,
		///StarRating::ReadOnly);
		////	QStyleOptionButton opt;
		////	opt.state |= QStyle::State_Enabled;
		////	if(option.state & QStyle::State_Selected)painter->fillRect(option.rect,
		///option.palette.highlight());
		////	opt.features	= opt.features | QStyleOptionButton::ButtonFeature::Flat
		///| QStyleOptionButton::ButtonFeature::CommandLinkButton;
		////	opt.rect	= option.rect.adjusted(1, 1, - 1, - 1);	// QRect(50, 25,
		///100, 50);//
		//////	//        auto title =
		///_view->record_ctrl()->source_model()->item(PosSource(PosProxy(index.row())))->field<name_type>();
		////	opt.text = "<b>" + it->field<name_type>() + "</b>";	// trUtf8("Button
		///text");
		////	_view->style()->drawControl(QStyle::CE_PushButton, &opt, painter, 0);
		/////	opt.paint(painter, option.rect, option.palette,
		///QStyleOptionButton::ReadOnly);
		// QStyleOptionViewItem optionV4 = option;
		// optionV4.state |= QStyle::State_Enabled;
		// initStyleOption(const_cast<QStyleOptionViewItem *>(&option), index);	//
		// initStyleOption(&optionV4, index);
		//////	QStyle *style = optionV4.widget ? optionV4.widget->style() :
		///QApplication::style();
		//////	_view->setStyle(optionV4.widget ? optionV4.widget->style() :
		///QApplication::style());
		// if(option.state & QStyle::State_Selected)painter->fillRect(option.rect,
		// option.palette.highlight());
		// optionV4.rect = option.rect.adjusted(0, 0, - 1, - 1);
		// QTextDocument doc;
		////    if(index ==
		///static_cast<QModelIndex>(source_model()->index([&](boost::intrusive_ptr<const
		///Linker> it){return it->host()->id() ==
		///source_model()->session_id();})))optionV4.text = "<b>" + optionV4.text +
		///"</b>";
		// optionV4.text = "<b>" + optionV4.text + "</b>";
		// doc.setHtml(optionV4.text);	// optionV4.text
		///// Painting item without text
		// optionV4.text = QString();
		////	style->
		// _view->style()->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter);
		// QAbstractTextDocumentLayout::PaintContext ctx;
		//// Highlighting text if item is selected
		// if(optionV4.state &
		// QStyle::State_Selected)ctx.palette.setColor(QPalette::Text,
		// optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));
		// if(0 == it->count_direct() && ! (optionV4.state &
		// QStyle::State_Selected))ctx.palette.setColor(QPalette::Text,
		// optionV4.palette.color(QPalette::Inactive, QPalette::Shadow));
		// QRect textRect =// style->
		// _view->style()->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);
		////	painter->restore();
		// painter->save();
		// painter->translate(QPoint(textRect.topLeft().x(), textRect.topLeft().y() -
		// 2));
		// painter->setClipRect(textRect.translated(- textRect.topLeft()));
		// doc.documentLayout()->draw(painter, ctx);
		// painter->restore();
		// #else
		// #endif
		// }
		else if (it && header_title == title_field_description) {
////
// try{
// QStyledItemDelegate::paint(painter, option, index);		// may crash?
// }catch(std::exception &e){
// qDebug() << "QStyledItemDelegate::paint(painter, option, index) : " <<
// e.what();
// }
////	drawFocus(painter, option, displayRect);

#ifdef USE_STAR_RATING

#elif defined USE_TEXT_AS_BUTTON
			//////        StarRating *star_rating = it->star_rating(); // =
			///qvariant_cast<StarRating>(index.data());
			////	if(option.state & QStyle::State_Selected)painter->fillRect(option.rect,
			///option.palette.highlight());
			//////        star_rating->paint(painter, option.rect, option.palette,
			///StarRating::ReadOnly);
			////	QStyleOptionButton opt;
			////	opt.state |= QStyle::State_Enabled;
			////	if(option.state & QStyle::State_Selected)painter->fillRect(option.rect,
			///option.palette.highlight());
			////	opt.features	= opt.features | QStyleOptionButton::ButtonFeature::Flat |
			///QStyleOptionButton::ButtonFeature::CommandLinkButton;
			////	opt.rect	= option.rect.adjusted(1, 1, - 1, - 1);	// QRect(50, 25, 100,
			///50);//
			//////	//        auto title =
			///_view->record_ctrl()->source_model()->item(PosSource(PosProxy(index.row())))->field<name_type>();
			////	opt.text = "<b>" + it->field<name_type>() + "</b>";	// trUtf8("Button
			///text");
			////	_view->style()->drawControl(QStyle::CE_PushButton, &opt, painter, 0);
			/////	opt.paint(painter, option.rect, option.palette,
			///QStyleOptionButton::ReadOnly);
			// std::shared_ptr<QStyleOptionViewItem> optionV4 =
			// std::make_shared<QStyleOptionViewItem>();	// = option;
			// optionV4->state |= QStyle::State_Enabled;
			// initStyleOption(optionV4.get(), index);	// initStyleOption(&optionV4,
			// index);	// terrible crash!
			//////	QStyle *style = optionV4.widget ? optionV4.widget->style() :
			///QApplication::style();
			//////	_view->setStyle(optionV4.widget ? optionV4.widget->style() :
			///QApplication::style());
			if (option.state & QStyle::State_Selected)
				painter->fillRect(option.rect, option.palette.highlight());
			// optionV4->rect = option.rect.adjusted(0, 0, - 1, - 1);
			QTextDocument doc;
			////    if(index ==
			///static_cast<QModelIndex>(source_model()->index([&](boost::intrusive_ptr<const
			///Linker> it){return it->host()->id() ==
			///source_model()->session_id();})))optionV4.text = "<b>" + optionV4.text +
			///"</b>";
			// optionV4->text = (it == _view->current_item()) ? "<b>" +
			// it->field<name_type>() + "</b>" : it->field<name_type>();	// (it ==
			// _view->current_item()) ? "<b>" + option.text + "</b>" : option.text;
			doc.setHtml((it == _view->current_item()) ? "<b>" + it->field<name_key>() + "</b>" : // [&] {auto rctrl = _view->record_ctrl();rctrl->select_as_current(rctrl->index<pos_proxy>(it));return "<b>" + it->field<name_type>() + "</b>";} ()
				it->field<name_key>());                                                      // doc.setHtml(optionV4.text);

			///// Painting item without text
			// optionV4->text = QString();
			////	style->
			//
			// _view->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);
			// // _view->style()->drawControl(QStyle::CE_ItemViewItem, optionV4.get(),
			// painter);

			QAbstractTextDocumentLayout::PaintContext ctx;
			// Highlighting text if item is selected
			if (option.state & QStyle::State_Selected)
				ctx.palette.setColor(QPalette::Text, option.palette.color(QPalette::Active, QPalette::HighlightedText)); // if(optionV4->state &
																	 // QStyle::State_Selected)ctx.palette.setColor(QPalette::Text,
																	 // optionV4->palette.color(QPalette::Active,
																	 // QPalette::HighlightedText));
			if (0 == it->count_direct() && !(option.state & QStyle::State_Selected) &&
			    (it != _view->current_item()))
				ctx.palette.setColor(QPalette::Text, option.palette.color(QPalette::Inactive, QPalette::Shadow)); // if(0 == it->count_direct() && ! (optionV4->state
																  // &
																  // QStyle::State_Selected))ctx.palette.setColor(QPalette::Text,
																  // optionV4->palette.color(QPalette::Inactive,
																  // QPalette::Shadow));
			QRect textRect =                                                                                          // style->
			    _view->style()->subElementRect(QStyle::SE_ItemViewItemText, &option);                                 // _view->style()->subElementRect(QStyle::SE_ItemViewItemText,
																  // optionV4.get());
			// painter->restore();
			painter->save();
			painter->translate(QPoint(textRect.topLeft().x(), textRect.topLeft().y() - 2));
			painter->setClipRect(textRect.translated(-textRect.topLeft()));
			doc.documentLayout()->draw(painter, ctx);
			// if(it == _view->current_item()){
			////	    _view->setFocus();
			// _record_ctrl->select_as_current(_record_ctrl->index<pos_proxy>(it));
			// }
			painter->restore();

#else

#endif
		}
	}
	// done
	painter->restore();
}

QSize ViewDelegation::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	(void)index;
#ifdef USE_STAR_RATING

	auto it = _view->record_ctrl()->source_model()->item(
	    pos_source(pos_proxy(index.row())));
	if (it) {
		StarRating* star_rating =
		    it->star_rating(); // StarRating star_rating =
				       // qvariant_cast<StarRating>(index.data());
		return star_rating->sizeHint();
	} else
#endif

		// QStyleOptionViewItem optionV4 = option;	// remove cosntant attribute
		// optionV4.state |= QStyle::State_Enabled;
		// initStyleOption(const_cast<QStyleOptionViewItem *>(&option), index);	//
		// initStyleOption(&optionV4, index);
		////    QStyle *style = optionV4.widget ? optionV4.widget->style() :
		///QApplication::style();
		////    _view->setStyle(optionV4.widget ? optionV4.widget->style() :
		///QApplication::style());
		////    if(option.state &
		///QStyle::State_Selected)painter->fillRect(option.rect,
		///option.palette.highlight());
		// optionV4.rect = option.rect.adjusted(1, 1, - 1, - 1);

		QTextDocument doc;
	doc.setHtml(option.text); // doc.setHtml(optionV4.text);
	doc.setTextWidth(
	    option.rect.width()); // doc.setTextWidth(optionV4.rect.width());

	return QSize(static_cast<int>(doc.idealWidth()), static_cast<int>(doc.size().height()));

	// return QStyledItemDelegate::sizeHint(option, index);
}

QWidget* ViewDelegation::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	(void)parent;
	(void)option;
	(void)index;
	QWidget* result = nullptr;

// auto	it		=
// _view->record_ctrl()->source_model()->item(pos_source(pos_proxy(index.row())));
// auto	header_title	=
// _view->record_ctrl()->source_model()->headerData(index.column(),
// Qt::Horizontal, Qt::DisplayRole).toString();			//
// DisplayRole?UserRole
// auto	rating_field_description	=
// fixedparameters.record_field_description(QStringList() <<
// boost::mpl::c_str<rating_type>::value)[boost::mpl::c_str < rating_type >
// ::value];
// auto	pin_field_description		=
// fixedparameters.record_field_description(QStringList() <<
// boost::mpl::c_str<pin_type>::value)[boost::mpl::c_str < pin_type > ::value];
// auto title_field_description =
// fixedparameters.record_field_description(QStringList() <<
// boost::mpl::c_str<name_type>::value)[boost::mpl::c_str < name_type >
// ::value];

#ifdef USE_STAR_RATING

	auto it = _view->record_ctrl()->source_model()->item(
	    pos_source(pos_proxy(index.row())));
	if (it) {
		// StarRating *star_rating = it->star_rating();
		FlatToolButtonRating* editor = new FlatToolButtonRating(parent);
		editor->raise();
		editor->show();
		connect(editor, &FlatToolButtonRating::clicked, this, &ViewDelegation::commitAndCloseEditor);
		connect(editor, &FlatToolButtonRating::clicked, _view->record_ctrl(), &rctl_t::close_context);
		connect(editor, &FlatToolButtonRating::editingFinished, this, &ViewDelegation::commitAndCloseEditor);
		connect(editor, &FlatToolButtonRating::editingFinished, _view->record_ctrl(), &rctl_t::close_context);

		return editor;
	} else
#elif defined USE_TEXT_AS_BUTTON
// if(it && header_title == title_field_description)result = new QLabel();
#else

#endif

		return result; // return QStyledItemDelegate::createEditor(parent, option,
			       // index);
}

void ViewDelegation::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	(void)editor;
	(void)model;
	(void)index;
#ifdef USE_STAR_RATING

	auto it = _view->record_ctrl()->source_model()->item(
	    pos_source(pos_proxy(index.row())));
	if (it) {
		// StarRating *star_rating = it->star_rating();
		FlatToolButtonRating* star_editor =
		    qobject_cast<FlatToolButtonRating*>(editor);
		model->setData(index, QVariant::fromValue(star_editor->star_rating()));
	} else
#endif
	// QStyledItemDelegate::setModelData(editor, model, index);
}

void ViewDelegation::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	(void)editor;
	(void)index;
#ifdef USE_STAR_RATING

	auto it = _view->record_ctrl()->source_model()->item(
	    pos_source(pos_proxy(index.row())));
	if (it) {
		StarRating* star_rating =
		    it->star_rating(); // StarRating star_rating =
				       // qvariant_cast<StarRating>(index.data());
		FlatToolButtonRating* starEditor =
		    qobject_cast<FlatToolButtonRating*>(editor);
		starEditor->star_rating(*star_rating);
	} else

#endif
	// QStyledItemDelegate::setEditorData(editor, index);
}

void ViewDelegation::commitAndCloseEditor()
{
#ifdef USE_STAR_RATING
	FlatToolButtonRating* editor = qobject_cast<FlatToolButtonRating*>(sender());
#else
	auto editor = dynamic_cast<QWidget*>(sender());
#endif

	emit commitData(editor);
	emit closeEditor(editor);
}

void ViewDelegation::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	(void)editor;
	(void)option;
	(void)index;
	// auto	it				=
	// _view->record_ctrl()->source_model()->item(pos_source(pos_proxy(index.row())));
	// auto	header_title			=
	// _view->record_ctrl()->source_model()->headerData(index.column(),
	// Qt::Horizontal, Qt::DisplayRole).toString();			//
	// DisplayRole?UserRole
	// auto	rating_field_description	=
	// fixedparameters.record_field_description(QStringList() <<
	// boost::mpl::c_str<rating_type>::value)[boost::mpl::c_str < rating_type >
	// ::value];
	// if(it && header_title == rating_field_description){		// index.column()
	// == 0
	// QRect	decorationRect	= QRect(option.rect.topLeft(),
	// QSize(option.rect.width(), option.rect.height()));	// QSize(80, 80)
	// QRect	displayRect	= QRect(decorationRect.topRight() + QPoint(1, 1),
	// QSize(150, 16));	// QSize(150, 25)//
	// QRect(decorationRect.topRight()+QPoint(20,30),QSize(50,50));
	// editor->setGeometry(displayRect);
	// }
	// editor->setGeometry(option.rect);
}

bool ViewDelegation::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	(void)event;
	(void)model;
	(void)option;
	(void)index;
	auto _record_ctrl = _view->record_ctrl();
	if (_record_ctrl) {
		auto it = _record_ctrl->source_model()->item(
		    _record_ctrl->index<pos_source>(_record_ctrl->index<pos_proxy>(index_proxy(index)))); // pos_source(pos_proxy(index.row()))
		auto header_title =
		    _record_ctrl->source_model()->headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString(); // DisplayRole?UserRole
		auto rating_field_description = fixedparameters.record_field_description(
		    QStringList() << boost::mpl::c_str<
			rating_key>::value)[boost::mpl::c_str<rating_key>::value];
		auto pin_field_description = fixedparameters.record_field_description(
		    QStringList() << boost::mpl::c_str<
			pin_key>::value)[boost::mpl::c_str<pin_key>::value];

#ifdef USE_STAR_RATING
#elif defined USE_TEXT_AS_BUTTON
		// and use the editorEvent to handle a click.
		if (event->type() == QEvent::MouseButtonRelease) {
			if (it && header_title == rating_field_description) {
				QMouseEvent* e = (QMouseEvent*)event;
				int clickX = e->x();
				int clickY = e->y();

				QRect r = option.rect; // getting the rect of the cell
				int x, y, w, h;
				x = r.left();   // r.left() + r.width() - _x_offset;	// the X
						// coordinate
				y = r.top();    // r.top() + 1;		// the Y coordinate
				w = r.width();  // _rating_width - _scroll_bar_width;		//
						// button width
				h = r.height(); // 16;	// button height
				if (clickX > x && clickX < x + w)
					if (clickY > y && clickY < y + h) {
						// QDialog *d = new QDialog();
						// d->setGeometry(0, 0, 100, 100);
						// d->show();
						if (_view->is_field_type_column(boost::mpl::c_str<rating_key>::value, index.column())) {
							////                auto widget = new FlatToolButton(this);
							////                setIndexWidget(next_index, widget);
							////                connect(widget, &FlatToolButton::clicked,
							///_record_controller, &RecordController::close_context);
							////
							///_record_controller->source_model()->setData(next_index,
							///QVariant(true), Qt::EditRole);
							// auto cur = current_item();
							// auto	_record_ctrl	= _view->record_ctrl();
							// auto it_pos =
							// _record_ctrl->source_model()->item(_record_ctrl->index<pos_source>(_record_ctrl->index<pos_proxy>(index_proxy(index))));
							// assert(cur == pos);
							_record_ctrl->remove(it->id()); // it_pos->id()
						}
					}
			} else if (it && header_title == pin_field_description) {
				if (it->field<pin_key>() != pin_value(bool_from_check_state[Qt::Unchecked]))
					it->field<pin_key>(pin_value(bool_from_check_state[Qt::Unchecked]));
				else
					it->field<pin_key>(pin_value(bool_from_check_state[Qt::Checked]));
			}
		}
#else
		// style()->drawControl(...)
		// and use the editorEvent to handle a click.
		if (event->type() == QEvent::MouseButtonRelease) {
			QMouseEvent* e = (QMouseEvent*)event;
			int clickX = e->x();
			int clickY = e->y();

			QRect r = option.rect; // getting the rect of the cell
			int x, y, w, h;
			x = r.left() + r.width() - _x_offset;  // the X coordinate
			y = r.top() + 1;                       // the Y coordinate
			w = _rating_width - _scroll_bar_width; // button width
			h = 16;                                // button height
			if (clickX > x && clickX < x + w)
				if (clickY > y && clickY < y + h) {
					// QDialog *d = new QDialog();
					// d->setGeometry(0, 0, 100, 100);
					// d->show();
					if (_view->_is_field_type_column(boost::mpl::c_str<rating_type>::value, index.column())) {
						////                auto widget = new FlatToolButton(this);
						////                setIndexWidget(next_index, widget);
						////                connect(widget, &FlatToolButton::clicked,
						///_record_controller, &RecordController::close_context);
						////
						///_record_controller->source_model()->setData(next_index,
						///QVariant(true), Qt::EditRole);
						// auto cur = current_item();
						auto _record_controller = _view->record_ctrl();
						auto pos = _record_controller->source_model()->item(
						    _record_controller->index<pos_source>(
							_record_controller->index<pos_proxy>(index_proxy(index))));
						// assert(cur == pos);
						_record_controller->remove(pos->id());
					}
				}
		}
#endif
	}
	return false;
}

// Виджет, отображащий список записей в ветке

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(rv_t)
#endif

rv_t::rv_t(rs_t* record_screen_, rctrl_t* record_ctrl_)
#ifdef USE_SIGNAL_CLOSE
    : super()
    , sd::renter() //sd::renter<rctrl_t>()
#else
    : super(record_screen_)
#endif // USE_SIGNAL_CLOSE
    , is_field_type_column([&](const QString& type_name, int index) -> bool {
	    // QString _type_name = boost::mpl::c_str<field_type>::value;
	    bool result = false;
	    RecordModel* rm = nullptr;
	    rm = _rctrl->source_model();
	    if (rm) {
		    auto header_title = rm->headerData(index, Qt::Horizontal, Qt::DisplayRole).toString(); // DisplayRole?UserRole
		    auto rating_field_description = fixedparameters.record_field_description(QStringList() << type_name)[type_name];

		    result = (header_title == rating_field_description);
	    }
	    return result;
    })
    , rating_width([&] { return _rating_width; })
    , _context_menu(new QMenu(this))
    , _record_screen(record_screen_)
    , _rctrl([&] {assert(record_ctrl_->proxy_model());this->setModel(record_ctrl_->proxy_model());return record_ctrl_; }())
    , _layout(new QVBoxLayout(this))
    , _delegate(new ViewDelegation(this)) // (new ButtonColumnDelegate(this))//
    , _rating_width(_delegate->_rating_width)
    , _vertical_scrollarea(new VerticalScrollArea(this, record_screen_))
{ // , _enable_move_section(true)
	// ViewDelegation *delegate = new ViewDelegation(this);
	setItemDelegate(_delegate);

	// setItemDelegateForColumn(2, delegate);
	setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
	// connect(this, &rv_t::clicked, this, [&](const QModelIndex
	// &index){itemDelegate(index)->show();});
	verticalHeader()->setDefaultAlignment(Qt::AlignmentFlag::AlignRight);
	// Пришлось ввести метод init, так как инициализация невозможна без
	// созданных в parent QAction, а создать в parent QAction можно только
	// при наличии ссылки на данный объект
	// Причина в том, что одни и те же QAction используются в двух местах -
	// в RecordTableScreen и здесь в контекстном меню

	auto setup_signals = [&, this](void) -> void { // RecordView::
		// Сигнал чтобы показать контекстное меню по правому клику на списке
		// записей
		connect(this, &rv_t::customContextMenuRequested, this, &rv_t::on_custom_context_menu_requested);

		// Соединение сигнал-слот чтобы показать контекстное меню по долгому
		// нажатию
		connect(this, &rv_t::tap_and_hold_gesture_finished, this, &rv_t::on_custom_context_menu_requested);

		// Сигнал чтобы открыть на редактирование параметры записи при двойном
		// клике
		// Signal to open for editing the parameters of the recording double click
		connect(this, &rv_t::doubleClicked, this, &rv_t::on_doubleclick);
		// connect(this, &RecordView::doubleClicked, _record_controller,
		// &RecordController::close_context); // _tab_widget

		// if(appconfig->interface_mode() == "desktop")
		// connect(this, &RecordView::list_selection_changed, this,
		// &RecordView::on_selection_changed);

		// Для мобильного режима должен работать сигнал clicked, так как если
		// засветка уже стоит на строке с записью, должна открыться запись
		// а в десктопном режиме этого не должно происходить, потому что запись
		// уже видна на экране
		// if(appconfig->getInterfaceMode() == "mobile")
		connect(this, &rv_t::clicked, this, &rv_t::on_click);

		// RecordScreen *_record_screen = qobject_cast<RecordScreen *>(parent());
		auto selectionmodel_ = this->selectionModel();
		assert(selectionmodel_);
		// Сигналы для обновления панели инструментов при изменении в
		// selectionModel()
		connect(selectionmodel_, &QItemSelectionModel::currentChanged, [&](const QModelIndex& current, const QModelIndex& previous) {
			(void)current;
			if (previous.isValid())
				_previous_index = previous;
			this->_record_screen->tools_update();
		});
		connect(selectionmodel_, &QItemSelectionModel::selectionChanged, [&](const QItemSelection& selected, const QItemSelection& deselected) {
			(void)selected;
			(void)deselected;
			this->_record_screen->tools_update();
		});

		// Сигналы для обновления панели инструментов
		connect(this, &rv_t::activated, this->_record_screen, &rs_t::tools_update);
		connect(this, &rv_t::clicked, this->_record_screen, &rs_t::tools_update);
		connect(this, &rv_t::doubleClicked, this->_record_screen, &rs_t::tools_update);
		connect(this, &rv_t::entered, this->_record_screen, &rs_t::tools_update);
		connect(this, &rv_t::pressed, this->_record_screen, &rs_t::tools_update);
		connect(QApplication::clipboard(), &QClipboard::dataChanged, this->_record_screen, &rs_t::tools_update);

		connect(this->horizontalHeader(), &QHeaderView::sectionMoved, this, &rv_t::on_section_moved);
		connect(this->horizontalHeader(), &QHeaderView::sectionResized, this, &rv_t::on_section_resized);
		if (this->_rctrl)
			connect(this->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this->_rctrl, &rctrl_t::on_sort_requested);

		destroy_transfer([&](sd::renter* const r) {
			(void)r;
			if (r != this) { //&& !this->_destroyed_request_sent
				this->close_trigger_from_others()(r);
				this->_destroy_request_sent = true;
				this->close();
			}
		});
	};

	auto init = [&, this](void) -> void { // RecordView::


		qDebug() << "RecordView::init()";
		// this->viewport()->installEventFilter(static_cast<QObject
		// *>(_record_screen->_vertical_scrollarea)); // should be
		// record_screen->_vertical_scrollarea?

		setup_signals();


		// It was previously Extended Selection, but this mode is not suitable for
		// Drag and Drop // Ранее было ExtendedSelection, но такой режим не подходит
		// для Drag and Drop
		this->setSelectionMode(
		    QAbstractItemView::SingleSelection // //ExtendedSelection //// MultiSelection// //ExtendedSelection
		    );
		auto selectionmodel_ = this->selectionModel();
		assert(selectionmodel_);
		this->selectionModel()->setModel(this->_rctrl->proxy_model());
		//                setSelectionModel();
		this->setSelectionBehavior(QAbstractItemView::SelectRows);

		this->restore_header_state();

		// астягивание последней секции до размеров виджета
		this->horizontalHeader()->setStretchLastSection(true);

		// Заголовки не должны выглядеть нажатыми
		this->horizontalHeader()->setHighlightSections(false);

// Горизонтальные заголовки делаются перемещяемыми
#if QT_VERSION >= 0x040000 && QT_VERSION < 0x050000
		this->horizontalHeader()->setMovable(true);
#endif
#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
		this->horizontalHeader()->setSectionsMovable(true);
#endif

		// Установка высоты строки с принудительной стилизацией (если это
		// необходимо),
		// так как стилизация через QSS для элементов QTableView полноценно не
		// работает
		// У таблицы есть вертикальные заголовки, для каждой строки, в которых
		// отображается номер строки.
		// При задании высоты вертикального заголовка, высота применяется и для всех
		// ячеек в строке.
		this->verticalHeader()->setDefaultSectionSize(
		    this->verticalHeader()->minimumSectionSize());
		int height = appconfig->ugly_qss_replace_height_for_table_view();
		if (height != 0)
			this->verticalHeader()->setDefaultSectionSize(height);
		if (appconfig->interface_mode() == "mobile")
			this->verticalHeader()->setDefaultSectionSize(calculate_iconsize_px());
		this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded); // setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
									   // // ScrollBarAsNeeded  //ScrollBarAlwaysOff

		this->restore_column_width();

		//// азрешается перемещать секции заголовка таблицы
		// _enable_move_section = true;

		// Нужно установить правила показа контекстного самодельного меню
		// чтобы оно могло вызываться
		//		assembly_context_menu(gl_paras->main_window()->main_menu_map());
		this->setContextMenuPolicy(Qt::CustomContextMenu);
	};

	setObjectName(record_view_multi_instance_name); // screen_name + "_view"
#ifdef USE_QT_DELETE_ON_CLOSE
	setAttribute(Qt::WA_DeleteOnClose, true);
#endif // USE_QT_DELETE_ON_CLOSE
	// Изначально сортировка запрещена (заголовки столбцов не будут иметь
	// треугольнички)
	this->setSortingEnabled(false);

	// Настройка области виджета для кинетической прокрутки
	set_kinetic_scrollarea(qobject_cast<QAbstractItemView*>(this));

	// азрешение принимать жест QTapAndHoldGesture
	grabGesture(Qt::TapAndHoldGesture);

	_layout->setSpacing(0);

	_layout->setMargin(0);

	// viewport()->getContentsMargins();
	auto selectionmodel_ = this->selectionModel();
	assert(selectionmodel_);
	init();
}

rv_t::~rv_t()
{
	// delete
	_context_menu->deleteLater();
	// delete
	_layout->deleteLater();
	if (_record_screen) { //never run
		auto _blogger = _record_screen->blogger();
		if (_blogger)
			if (!_blogger->close_request_sent() && !_blogger->destroy_request_sent())
				_blogger->destroy_trigger_from_others()(this);
	}
}

// void RecordTableView::setController(RecordTableController *pController)
// {
// _controller = pController;
// }

void rv_t::assembly_context_menu(const std::map<std::string, QMenu*>& main_menu_map_)
{ // RecordView::
//// Конструирование меню
// _context_menu = new QMenu(this);

// RecordTableScreen *_recordtablescreen = qobject_cast<RecordTableScreen
// *>(parent());

// _context_menu->addAction(_record_screen->_record_hide);	// move to
// main_window::_vtab_record->tabBar()->tabBarClicked
#ifdef USE_BUTTON_PIN
	_context_menu->addAction(_record_screen->_pin);
#endif
#ifdef USE_BLANK_ITEM
	_context_menu->addAction(_record_screen->_addnew_to_end);
#endif
	_context_menu->addAction(_record_screen->_edit_field);
#ifdef USE_BUTTON_CLOSE
	_context_menu->addAction(_record_screen->_close);
#endif

	_context_menu->addSeparator();
#ifdef USE_BLANK_ITEM
	_context_menu->addAction(_record_screen->_addnew_before);
	_context_menu->addAction(_record_screen->_addnew_after);
#endif
	_context_menu->addAction(_record_screen->_action_move_top);
	_context_menu->addAction(_record_screen->_action_move_up);
	_context_menu->addAction(_record_screen->_action_move_dn);
	_context_menu->addAction(_record_screen->_action_syncro);

// _context_menu->addSeparator();
#ifdef USE_WITHOUT_REGISTERED_TREEITEM
	_context_menu->addAction(_record_screen->_cut);
	_context_menu->addAction(_record_screen->_copy);
	_context_menu->addAction(_record_screen->_paste);
#endif
	_context_menu->addAction(_record_screen->_delete);

	_context_menu->addSeparator();

	_context_menu->addAction(_record_screen->_sort);
	_context_menu->addAction(_record_screen->_print);
	_context_menu->addAction(
	    _record_screen->_editor); // connect(this, &RecordView::doubleClicked,
				      // this, &RecordView::on_doubleclick);
	_context_menu->addAction(_record_screen->_settings);

	_context_menu->addSeparator();
	{
		// auto main_menu_action = _context_menu->addSection("main menu");
		// main_menu_action->setMenu(_main_menu_in_button);            //
		// _context_menu->addAction(_main_menu_action);
		// _context_menu->addAction(_record_screen->tree_screen()->_actionlist[action_main_menu]);
		//		auto main_menu_map_ = gl_paras->main_window()->main_menu_map();
		for (auto menu : main_menu_map_) _context_menu->addMenu(menu.second);
	}
}

rctrl_t* rv_t::record_ctrl()
{
	return _rctrl;
}

//QModelIndex rv_t::previous_index() const
//{
//	return _previous_index;
//}

void rv_t::restore_header_state(void)
{
	// Видимость горизонтальных заголовков
	if (appconfig->record_table_show_horizontal_headers() == false)
		horizontalHeader()->hide();
	else
		horizontalHeader()->show();
	// Видимость вертикальных заголовков
	if (appconfig->record_table_show_vertical_headers() == false)
		verticalHeader()->hide();
	else
		verticalHeader()->show();
}

// void RecordView::on_selection_changed(const QItemSelection &selected, const
// QItemSelection &deselected)
// {
// QModelIndex select_record_index;
// QModelIndex deselect_record_index;

// if(!selected.indexes().isEmpty())
// select_record_index = selected.indexes().at(0);

// if(!deselected.indexes().isEmpty())
// deselect_record_index = deselected.indexes().at(0);

////    qDebug() << "RecordTableView::onSelectionChanged()";
////    qDebug() << "Current index. row() " << selectRecord.row() << " isValid()
///" << selectRecord.isValid();
////    qDebug() << "Previous index. row() " << deselectRecord.row() << "
///isValid() " << deselectRecord.isValid();

//// return;

////    if(select_record_index.isValid())
////        click_record(select_record_index);    // this lead
///RecordScreen::disable_all_actions does not work correctly
// }

// Слот клика по записи. Принимает индекс Proxy модели
void rv_t::on_click(const QModelIndex& index_proxy_)
{
	// setSelectionMode(QAbstractItemView::SingleSelection);// ExtendedSelection
	if (index_proxy_.isValid()) {
		if ( //&&
		    _previous_index != index_proxy_) _previous_index = index_proxy_;
		if (_rctrl)
			_rctrl->index_invoke(index_proxy(index_proxy_));
	}
}

// Слот, срабатывающий при нажатии кнопки редактирования записи
void rv_t::on_doubleclick(const QModelIndex& index)
{
	qDebug() << "In RecordTableView::editFieldContext";

	// Получение индекса выделенного элемента
	//	QModelIndexList selectItems = selectionModel()->selectedIndexes();
	//	if (selectItems.size() > 0) {
	// QModelIndex index = selectItems.at(0);

	// globalparameters.getMetaEditor()->switch_pin();

	// controller->open_website(index); //controller->editFieldContext(index);

	// Нужно перерисовать окно редактирования чтобы обновились инфополя
	// делается это путем "повторного" выбора текущего пункта
	if (_rctrl)
		_rctrl->index_invoke(index_proxy(index), true); // force to refresh		// аньше было select()
	// globalparameters.main_window()->editor_switch();
	//	}
}

// Слот, срабатывающий после того, как был передвинут горизонтальный заголовок
void rv_t::on_section_moved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
	Q_UNUSED(logicalIndex);
	if (!_enable_move_section)
		return;
	// Если была включена сортировка
	/*
                 if( this->isSortingEnabled() )
                 if( horizontalHeader()->sortIndicatorSection())
                  horizontalHeader()->setSortIndicator(n, Qt::AscendingOrder);
     // Треугольничек сортировки переставляется на нужный столбец
       */

	// Запоминается ширина столбцов
	int oldVisualWidth = horizontalHeader()->sectionSize(oldVisualIndex);
	int newVisualWidth = horizontalHeader()->sectionSize(newVisualIndex);

	// В настройках последовательность полей меняется
	QStringList showFields = appconfig->record_table_show_fields();
	showFields.move(oldVisualIndex, newVisualIndex);
	appconfig->record_table_show_fields(showFields);

	qDebug() << "New show field sequence" << showFields;

	_enable_move_section = false;
	// Перемещение в данном представлении сбрасывается, так как модель берет
	// последовательность полей из настроек
	// После это кода logicalIindex=visualIndex для всех полей
	for (int logicalIdx = 0; logicalIdx < showFields.size(); logicalIdx++) {
		int visualIdx = horizontalHeader()->visualIndex(logicalIdx);
		if (visualIdx != logicalIdx)
			horizontalHeader()->moveSection(
			    visualIdx, logicalIdx); // Этот вызов запустит срабатывание этого же
						    // слота sectionMoved(), поэтому нужен
						    // enableMoveSection
	}
	_enable_move_section = true;

	horizontalHeader()->reset();

	// Устанавливается ширина столбцов после перемещения
	horizontalHeader()->resizeSection(oldVisualIndex, newVisualWidth);
	horizontalHeader()->resizeSection(newVisualIndex, oldVisualWidth);

	save_column_width();
}

void rv_t::on_section_resized(int logicalIndex, int oldSize, int newSize)
{
	Q_UNUSED(logicalIndex);
	Q_UNUSED(oldSize);
	Q_UNUSED(newSize);

	save_column_width();
}

//// Действия при выборе строки таблицы конечных записей. Принимает индекс Proxy
///модели
//// Actions when choosing the final row of the table entries. Accepts index
///Proxy models
// void RecordView::click_record(const IndexProxy &proxy_index)
// {
// _record_controller->item_click(proxy_index);

////    globalparameters.window_switcher()->switchFromRecordtableToRecord();  //
///move to item_click
// }

// Открытие контекстного меню в таблице конечных записей
void rv_t::on_custom_context_menu_requested(const QPoint& pos)
{
	qDebug() << "In on_customContextMenuRequested";
	// auto p = parent();
	// RecordTableScreen *parentPointer = qobject_cast<RecordTableScreen
	// *>(parent());
	// RecordTableScreen *parentPointer = static_cast<RecordTableScreen *>(p);
	// RecordTableScreen *_recordtablescreen =
	// globalparameters.getRecordTableScreen();

	//// Устанавливается надпись для пункта сортировки
	// if(! this->isSortingEnabled())_record_screen->_sort->setText(tr("Enable
	// sorting"));
	// else _record_screen->_sort->setText(tr("Disable sorting"));

	// Запоминается номер колонки, по которой был произведен клик (номер колонки
	// будет правильный, даже если записей мало и клик произошел под записями)
	int n = this->horizontalHeader()->logicalIndexAt(pos);
	qDebug() << "Click on column number " << n;
	_record_screen->_sort->setData(
	    n); // Запоминается в объект действия для сортировки

	// Включение отображения меню на экране
	// menu.exec(event->globalPos());
	_context_menu->exec(viewport()->mapToGlobal(pos));
}

// Слот, срабатывающий при нажатии кнопки редактирования записи
void rv_t::edit_field_context(void)
{
	qDebug() << "In RecordView::edit_field_context";

	//// Получение индекса выделенного элемента
	// QModelIndexList	selectItems	= selectionModel()->selectedIndexes();
	// auto		current_index	= currentIndex();
	index_proxy proxy_index;
	// if(current_index.isValid()){
	// for(auto &si : selectItems)
	// if(0 == si.column()) proxy_index = index_proxy(si);
	// // selectItems.at(0);
	//// auto _index0 =
	///_record_controller->proxyindex_to_sourceindex(proxy_index);
	//// auto _index1 =
	///_record_controller->sourceindex_to_proxyindex(proxy_index);  // invalid
	//// proxy_index = _record_controller->pos_to_proxyindex(proxy_index.row());
	////    if(! ((QModelIndex) proxy_index).isValid())
	// }else{
	auto c_i = current_item();
	//	auto _rctrl = _rctrl;
	if (_rctrl) {

		if (!currentIndex().isValid())

			_rctrl->select_as_current(_rctrl->index<pos_proxy>(c_i));
		proxy_index = _rctrl->index<index_proxy>(c_i);
		// current_index	= currentIndex();
		// }
		// assert(((QModelIndex) proxy_index).row() == current_index.row());
		if (_rctrl->edit_field_context(proxy_index)) { // proxy_index
			// Нужно перерисовать окно редактирования чтобы обновились инфополя
			// делается это путем "повторного" выбора текущего пункта
			_rctrl->index_invoke(proxy_index); // proxy_index // Раньше было select()
		}
	}
}

//// Получение номера первого выделенного элемента
// PosProxy RecordView::selection_first_pos(void)const
// {
//// Получение списка выделенных Item-элементов
// QModelIndexList selectItems = selectionModel()->selectedIndexes();

// if(selectItems.isEmpty())
// return PosProxy(-1); // Если ничего не выделено
// else
// return PosProxy((selectItems.at(0)).row()); // Номер первого выделенного
// элемента
// }

//// Получение ID первого выделенного элемента
// IdType RecordView::selection_first_id(void)const
// {
//// Получение списка выделенных Item-элементов
// QModelIndexList selectItems = selectionModel()->selectedIndexes();

// if(selectItems.isEmpty())
// return IdType(""); // Если ничего не выделено

// return IdType(selectItems.at(0).data(RECORD_ID_ROLE).toString());
// }

//// Получение модельного индекса первого выделенного элемента в Proxy модели
// IndexProxy RecordView::selection_first_proxy_index(void)const
// {
// PosProxy pos = selection_first<PosProxy>();

// if(pos == -1)
// return IndexProxy(QModelIndex());

//// QModelIndex index = recordProxyModel->index( pos, 0 );
// IndexProxy index = _record_controller->index_proxy(PosProxy(pos));

// return index;
// }

//// Получение модельного индекса первого выделенного элемента в Source модели
// IndexSource RecordView::selection_first_source_index(void)const
// {
// IndexProxy proxy_index = selection_first<IndexProxy>();

// if(!((QModelIndex)proxy_index).isValid())
// return IndexSource(QModelIndex());

//// QModelIndex index = recordProxyModel->mapToSource( proxyIndex );
// IndexSource index = _record_controller->index_source(proxy_index);

// return index;
// }

boost::intrusive_ptr<i_t> rv_t::current_item() const
{
	boost::intrusive_ptr<i_t> it;
	//	auto _rctrl = _rctrl;
	if (_rctrl) {
		if (_rctrl->source_model()->count() > 0) {
			it = _rctrl->source_model()->current_item();
			if (it) {
				pos_proxy pos_proxy_ = _rctrl->index<pos_proxy>(it);
////	pos_source	possource	=
///_record_controller->index<pos_source>(it);
////	auto		index		=
///static_cast<QModelIndex>(_record_controller->index<index_source>(posproxy));
// index_proxy proxy_curi(selectionModel()->currentIndex());
////	auto	source_index	=
///_record_controller->source_model()->current_index();
////	auto	proxy_index	=
///_record_controller->proxy_model()->mapFromSource(static_cast<QModelIndex>(source_index));
////	assert(proxy_curi == proxy_index);
#ifdef USE_HAS_SELECTION
				QItemSelectionModel* item_selection_model = selectionModel();
				bool has_selection = item_selection_model->hasSelection();
#endif // USE_HAS_SELECTION
				if (pos_proxy_ != static_cast<QModelIndex>(_rctrl->index<index_proxy>(_rctrl->source_model()->current_index())).row()
#ifdef USE_HAS_SELECTION
				    || (!has_selection)
#endif                                      // USE_HAS_SELECTION
					) { //
					// if(pos_proxy_ != static_cast<QModelIndex>(proxy_curi).row()){	//
					// never equaled	// currentIndex().row()){	//
					// selectionModel()->currentIndex().row()
					//
					// const_cast<rv_t *>(this)->reset();
					// _record_controller->proxy_model()->setSourceModel(_record_controller->source_model());
					// const_cast<rv_t *>(this)->setModel(_record_controller->proxy_model());

					_rctrl->select_as_current(pos_proxy_);
				}
			}
		}
	}
	return it;
}

bool rv_t::is_selected_set_to_top(void)
{
	return selection_first<pos_proxy>() == pos_proxy(0);
}

bool rv_t::is_selected_set_to_bottom(void)
{
	return selection_first<pos_proxy>() == pos_proxy(model()->rowCount() - 1);
}

//// mode - режим в котором добавлялась новая запись
//// pos - позиция новой записи в размерности Source модели
// void RecordView::cursor_to_index(PosProxy pos_proxy_, int mode)
// {
//// Установка курсора на только что созданную позицию

////    QModelIndex selIdx = recordSourceModel->index(selPos, 0); // Создание
///индекса из номера
////    selectionModel()->setCurrentIndex(selIdx,
///QItemSelectionModel::ClearAndSelect);

//// В QTableView некорректно работает установка на только что созданную строку
//// Это как-то связано с отрисовкой виджета QTableView
//// Прокрутка к только что созданной строке через selectRow() показывает только
//// верхнюю часть новой строки. Чтобы этого избежать, при добавлении в конец
//// таблицы конечных записей, установка прокрутки делается через
///scrollToBottom()
// if(mode == add_new_record_to_end
// || (mode == add_new_record_after && pos_proxy_ >= (model()->rowCount() - 1))
// ) {
// scrollToBottom();
// }

// PosProxy
// proxy_pos(((QModelIndex)_record_controller->index_proxy(pos_proxy_)).row());

// selectRow((int)proxy_pos);

// IndexProxy index = _record_controller->index_proxy(pos_proxy_);
// selectionModel()->select((QModelIndex)index, current_tree_selection_mode);
//// Установка засветки на нужный индекс
//// Set the backlight to the desired index
// selectionModel()->setCurrentIndex((QModelIndex)index   // selIdx
// , current_tree_current_index_mode // QItemSelectionModel::Select    //
// ClearAndSelect
// );

// }

bool rv_t::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonDblClick &&
	    static_cast<QMouseEvent*>(event)->button() == Qt::LeftButton)
		on_doubleclick(indexAt(static_cast<QMouseEvent*>(event)->pos()));
	return QTableView::eventFilter(obj, event);
}

// Обработчик событий, нужен только для QTapAndHoldGesture (долгое нажатие)
bool rv_t::event(QEvent* event)
{
	bool result = false;
	if (event) {
		if (!event->spontaneous()) {
			if (event->type() == QEvent::Gesture) {
				qDebug() << "In gesture event(): " << event
					 << " Event type: " << event->type();

				return gesture_event(static_cast<QGestureEvent*>(event));
			} else if (event->type() == QEvent::MouseButtonDblClick) {
				on_doubleclick(indexAt(static_cast<QMouseEvent*>(event)->pos()));
				// return true;
				result = true;
			} else {
				// you can not comment out the block, it will make cpu usage always high
				if (event->type() <= QEvent::Type::MaxUser) {
					try {
						result = QTableView::event(event); // sometimes halt the system
					} catch (std::exception& e) {
						qDebug() << "bool rv_t::event(QEvent *event): " << e.what();
					}
				}
				// return QTableView::event(event);
			}
		} else
			result = QTableView::event(event); // else  return false;
	}
	return result;
}

// Обработчик жестов
// Вызывается из обработчика событий
bool rv_t::gesture_event(QGestureEvent* event)
{
	qDebug() << "In gestureEvent()" << event;
	if (QGesture* gesture = event->gesture(Qt::TapAndHoldGesture))
		tap_and_hold_gesture_triggered(static_cast<QTapAndHoldGesture*>(gesture));
	return true;
}

// Обработчик жеста TapAndHoldGesture
// Вызывается из обработчика жестов
void rv_t::tap_and_hold_gesture_triggered(QTapAndHoldGesture* gesture)
{
	qDebug() << "In tapAndHoldGestureTriggered()" << gesture;
	if (gesture->state() == Qt::GestureFinished)
		if (gl_paras->target_os() == "android")
			emit tap_and_hold_gesture_finished(
			    mapFromGlobal(gesture->position().toPoint()));
}

// еакция на нажатие кнопок мышки
void rv_t::mousePressEvent(QMouseEvent* event)
{
	// ts_t * = globalparameters.tree_screen();		// static_cast<TreeScreen
	// *>(this->parent());
	////    // get the buttons type
	////    Qt::MouseButtons mouse_button = event->buttons();
	// setSelectionMode(QAbstractItemView::ExtendedSelection);
	_mouse_start_position = event->pos();
	auto mouse_button = event->button();
	QModelIndex next_index = indexAt(_mouse_start_position);
	if (next_index.isValid() //
	    //	    && !is_field_type_column(boost::mpl::c_str<rating_key>::value, next_index.column()) //
	    //	    && mouse_button == Qt::RightButton
	    ) {
		////        // Если нажата левая кнопка мыши
		////        if(mouse_button == Qt::LeftButton){ // || mouse_button == Qt::RightButton

		////            selectionModel()->select(next_index, QItemSelectionModel::SelectCurrent);
		//////
		if (mouse_button == Qt::LeftButton && is_field_type_column(boost::mpl::c_str<rating_key>::value, next_index.column())) {
			if (_rctrl) _rctrl->source_model()->setData(next_index, QVariant(true), Qt::EditRole);
			//////                auto widget = new FlatToolButton(this);
			//////                setIndexWidget(next_index, widget);
			//////                connect(widget, &FlatToolButton::clicked, _record_controller, &RecordController::close_context);
			//////            }
			////            emit dataChanged(next_index, next_index);
			////            // auto it = _record_controller->source_model()->item(next_index.row());
			////            // _tree_screen->tree_view()->select_as_current(it);
		} else if (mouse_button == Qt::RightButton) {
			// only the right mouse buton
			////            // _mouse_start_position = event->pos();
			////            ////select item at cursor position
			////            ////        QPersistentModelIndex
			////            // QModelIndex next_index = indexAt(event->pos());
			////            // selectionModel()->clear();
			////            selectionModel()->select(next_index, QItemSelectionModel::ClearAndSelect); // Select
			////            selectionModel()->setCurrentIndex(next_index, QItemSelectionModel::SelectCurrent);
			////            assert(next_index == currentIndex());
			//// _record_controller->cursor_to_index(PosProxy(next_index.row())); //
			////            // emit clicked(next_index);

			if (_rctrl) {
				auto it = _rctrl->source_model()->item(_rctrl->index<pos_source>(_rctrl->index<pos_proxy>(index_proxy(next_index))));    // pos_source(pos_proxy(index.row()))
				auto header_title = _rctrl->source_model()->headerData(next_index.column(), Qt::Horizontal, Qt::DisplayRole).toString(); // DisplayRole?UserRole
				auto rating_field_description = fixedparameters.record_field_description(QStringList() << boost::mpl::c_str<rating_key>::value)[boost::mpl::c_str<rating_key>::value];

				if (it) {
#ifdef USE_HAS_SELECTION
					auto _record_view = _rctrl->view();
					QItemSelectionModel* item_selection_model = _record_view->selectionModel();
					bool has_selection = item_selection_model->hasSelection();
#endif // USE_HAS_SELECTION
					if (header_title != rating_field_description
#ifdef USE_HAS_SELECTION
					    || !has_selection
#endif // USE_HAS_SELECTION
					    ) {
						_rctrl->select_as_current(_rctrl->index<pos_proxy>(index_proxy(next_index)));
					}
				}
			}

			////            //// start the context menu
			////            // QModelIndexList select_indexes(selectedIndexes());

			////            // if(select_indexes.size() > 0 &&  select_indexes[0].isValid()) {
			////            // _tree_screen->_context_menu->exec(event->pos());  // QCursor::pos()
			////            // }
			////        }
		}
		//		//	selectionModel()->select(next_index, QItemSelectionModel::ClearAndSelect);			// Select
		//		//	selectionModel()->setCurrentIndex(next_index, QItemSelectionModel::SelectCurrent);
		//		//	assert(next_index == currentIndex());
		//		_rctrl->index_invoke(index_proxy(next_index), true); // .row()

		//		//	auto it = _record_controller->source_model()->item(_record_controller->index<pos_source>(_record_controller->index<pos_proxy>(index_proxy(next_index)))); // .row()
		//		//	assert(it);
		//		//	auto tree_view = _tree_screen->view();

		//		//	tree_view->select_as_current(TreeIndex::instance([&] {return tree_view->source_model();}, it, it->parent()));
		//		//	assert(tree_view->current_item() == it);
		//		//	assert(current_item() == it);
	}
	// else {
	////call the parents function
	// QTableView::mousePressEvent(event);
	// }
	//	//	auto _rctrl = _rctrl;

	QTableView::mousePressEvent(event);
}

// еакция на движение мышкой
void rv_t::mouseMoveEvent(QMouseEvent* event)
{
	// Если при движении нажата левая кнопка мышки
	if (event->buttons() & Qt::LeftButton) {
		// Выясняется расстояние от места начала нажатия
		int distance = (event->pos() - _mouse_start_position).manhattanLength();
		if (distance >= QApplication::startDragDistance() * 2)
			start_drag();
	}
	QTableView::mouseMoveEvent(event);
}

// еакция на отпускание клавиши мышки
void rv_t::mouseReleaseEvent(QMouseEvent* event)
{
	////    ts_t *_tree_screen = globalparameters.tree_screen();
	//////    Qt::MouseButtons mouse_button = event->buttons();
	// QModelIndex next_index = indexAt(event->pos());
	// if(next_index.isValid()){
	////        if(mouse_button == Qt::LeftButton){
	// if(_is_field_type_column(boost::mpl::c_str<rating_type>::value,
	// next_index.column())){
	//////                auto widget = new FlatToolButton(this);
	//////                setIndexWidget(next_index, widget);
	//////                connect(widget, &FlatToolButton::clicked,
	///_record_controller, &RecordController::close_context);
	//////            _record_controller->source_model()->setData(next_index,
	///QVariant(true), Qt::EditRole);
	////            auto cur = current_item();
	// auto pos =
	// _record_controller->source_model()->item(_record_controller->index<pos_source>(_record_controller->index<pos_proxy>(index_proxy(next_index))));
	////            assert(cur == pos);
	// _record_controller->remove(pos->id());
	// }
	////	else{
	//////	    selectionModel()->select(next_index,
	///QItemSelectionModel::ClearAndSelect);				//
	///Select
	//////	    selectionModel()->setCurrentIndex(next_index,
	///QItemSelectionModel::SelectCurrent);
	//////	    assert(next_index == currentIndex());
	////	    _record_controller->index_invoke(index_proxy(next_index), true);
	///// .row()

	//////	    auto it =
	///_record_controller->source_model()->item(_record_controller->index<pos_source>(_record_controller->index<pos_proxy>(index_proxy(next_index))));
	///// .row()
	//////	    assert(it);
	//////	    auto tree_view = _tree_screen->view();

	//////	    tree_view->select_as_current(TreeIndex::instance([&] {return
	///tree_view->source_model();}, it, it->parent()));
	//////	    assert(tree_view->current_item() == it);
	//////	    assert(current_item() == it);
	////	}
	// }
	QTableView::mouseReleaseEvent(event);
}

// #ifndef QT_NO_WHEELEVENT
//// move to VerticalScrollArea::wheelEvent
// void rv_t::wheelEvent(QWheelEvent *event){
// (void) event;
////    x -= (event->delta() / 120) * 10;
////    verticalScrollBar()->setValue(x);
// }
// #endif

// Начало переноса записи
void rv_t::start_drag()
{
	qDebug() << "Start record drag\n";
	// Если действительно выбрана строка
	if (currentIndex().isValid()) {
		// Перед переносом нужно запомнить текст последней редактируемой записи,
		// чтобы не перенесся неотредактированный вариант
		// find_object<MainWindow>("mainwindow")
		_record_screen->blogger()->save_text_context();

		// Копирование выделенных строк в объект переноса
		QDrag* drag = new QDrag(this);
		drag->setMimeData(get_selected_records());

		// Запуск операции перетаскивания объекта
		unsigned int result = drag->exec(Qt::MoveAction);
		// Если перетаскивание завершено
		if (result == 0) {
			// Сюда код дойдет после того, как перетаскивание будет закончено
			qDebug() << "Success drag and drop move record";

			// todo: Совершенно непонятно, где удалять объект drag.
			// Если удалять в этом месте, имеем сегфолт
			// delete drag;

			// В модели данных обнуляется оформление элемента, который (возможно)
			// подсвечивался при Drag And Drop
			// find_object<TreeScreen>(tree_screen_singleton_name)
			gl_paras->tree_screen()->view()->source_model()->setData(
			    QModelIndex(), QVariant(false), Qt::UserRole);
		}
	}
}

ClipboardRecords* rv_t::get_selected_records(void)
{
	// Получение списка Item-элементов, подлежащих копированию
	QModelIndexList indexes_for_copy = selectionModel()->selectedIndexes();

	// В списке должны остаться только элементы столбца 0
	// (так как ранее одна строка была одним элементом списка,
	// а теперь используется таблица, и при одной выделенной строке
	// выделено несколько элементов)
	// The list should only elements of the column 0
	// (As earlier one line was one element of the list,
	// And now uses a table, and a dedicated line
	// Select multiple items)
	QMutableListIterator<QModelIndex> iterator(indexes_for_copy);
	while (iterator.hasNext()) {
		iterator.next();

		QModelIndex index = iterator.value();
		if (index.column() > 0)
			iterator.remove();
	}
	// Список возвращается в произвольном порядке, не в таком как на экране
	// поэтому его нужно отсортировать по QModelIndex
	qSort(indexes_for_copy.begin(), indexes_for_copy.end());

	qDebug() << "Get selected records";
	for (int i = 0; i < indexes_for_copy.size(); ++i)
		qDebug() << indexes_for_copy.at(i).data().toString();
	// Объект с данными для заполнения буфера обмена
	ClipboardRecords* clipboardRecords = new ClipboardRecords();
	clipboardRecords->clear();

	//	auto _rctrl = _rctrl;
	if (_rctrl) {
		// Объект заполняется выбранными записями
		_rctrl->add_items_to_clipboard(clipboardRecords, indexes_for_copy);
	}
	return clipboardRecords;
}

// Переопределенный сигнал (virtual protected slot)
void rv_t::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	// qDebug() << "RecordView::selectionChanged()";

	// emit list_selection_changed(selected, deselected);

	// Для корректной работы надо вызвать сигнал базового класса
	QTableView::selectionChanged(selected, deselected);
}

// Сохранение ширины колонок в конфигфайл
void rv_t::save_column_width(void)
{
	// Выясняется количество полей
	int count = appconfig->record_table_show_fields().size();

	QStringList columnWidthList;
	// if(appconfig->record_table_show_vertical_headers())columnWidthList <<
	// QString::number(verticalHeader()->geometry().width());	// line number
	// width
	for (int i = 0; i < count; i++) {
		QString width;
		width.setNum(columnWidth(i));
		columnWidthList << width;
	}
	appconfig->record_table_fields_width(columnWidthList);

	// qDebug() << "Save column width " << columnWidthList;
}

// Восстановление ширины колонок из конфигфайла
void rv_t::restore_column_width(void)
{
	QStringList columnWidthList = appconfig->record_table_fields_width();
	// qDebug() << "Restore column width " << columnWidthList;
	// Восстанавливается ширина всех колонок без последней
	// Чтобы последняя растягивалась по месту
	for (int i = 0; i < columnWidthList.size() - 1; i++)
		setColumnWidth(i, columnWidthList[i].toInt());
	resizeEvent(nullptr);
}

//// if pin and title width beyond container width, when click title item,
///widget will move left, pin column will disappeared
// void RecordTableView::on_parent_resizevent(QResizeEvent *e)
// {
// Q_UNUSED(e)

////    int vscrollbar_width = this->verticalScrollBar()->width();
////    int viewport_width = this->maximumViewportSize().width();
////    int _width = width();
////    int parent_width = e->size().width();
////    auto margin = viewport()->contentsMargins();
////    auto rect =  viewport()->contentsRect();
// auto rect1 = contentsRect();
// auto real_width = rect1.width();
////    auto size_width = size().width();

// if((columnWidth(0) + columnWidth(1)) >= real_width) {
// setColumnWidth(0, 30);
// setColumnWidth(1, (real_width >= 30) ? real_width - 30 : columnWidth(1));
// } else {
// setColumnWidth(0, 30);
// real_width = this->contentsRect().width();

// if(real_width >= 300) {
// setColumnWidth(1, 300 - 30); // restoreColumnWidth();
// } else {
// setColumnWidth(1, (real_width >= 30) ? real_width - 30 : columnWidth(1));
// }
// }

////    saveColumnWidth();
// }

// int rv_t::vertical_scroll_bar_width()const{return
// _vertical_scroll_bar_width;}
// void rv_t::vertical_scroll_bar_width(int w){_vertical_scroll_bar_width = w;}

// if pin and title width beyond container width, when click title item, widget
// will move left, pin column will disappeared
void rv_t::resizeEvent(QResizeEvent* e)
{
	// auto is_vertical_scrollbar_visible = [&]() const->bool {
	// bool IsVisible = false;

	// int HeightOfAllRows = 0;
	// for(int i = 0; i < _record_controller->source_model()->count(); i
	// ++)HeightOfAllRows += rowHeight(i);
	// int	HeaderHeight	= horizontalHeader()->height();
	// int	TableHeight	= height();
	// if((HeightOfAllRows + HeaderHeight) > TableHeight)IsVisible = true;
	// return IsVisible;
	// };
	//// Q_UNUSED(e)
	{
		// int vscrollbar_width = this->verticalScrollBar()->width();
		// int viewport_width = this->maximumViewportSize().width();
		// int _width = width();
		// int parent_width = e->size().width();
		// auto margin = viewport()->contentsMargins();
		// auto rect0 =  viewport()->contentsRect();
		auto rect = contentsRect();
		auto real_capacity = rect.width();
		auto row_number_width = appconfig->record_table_show_vertical_headers() ? verticalHeader()->geometry().width() : 0;
		// auto	_vertical_scroll_bar_width	=
		// _vertical_scroll_bar_width;//verticalScrollBar()->width();
		// bool	_is_vertical_scrollbar_visible		=
		// is_vertical_scrollbar_visible();
		bool _is_vertival_scroll_bar_visibale = verticalScrollBar()->isVisible();
		// bool	_vertical_scroll_is_visible		= _vertical_scroll_area ?
		// _vertical_scroll_area->verticalScrollBar()->isVisible() : false;

		int scroll_sapce_width =
		    _is_vertival_scroll_bar_visibale ? 0 : _scroll_bar_width;
		rating_width = [&] { return _rating_width + scroll_sapce_width; };
		_delegate->_x_offset =
		    _is_vertival_scroll_bar_visibale ? (_delegate->_rating_width - 2 * _scroll_bar_width / 3) : (_delegate->_rating_width);
		int suggest_others_width =
		    row_number_width + _pin_width + rating_width() +
		    (_scroll_bar_width - scroll_sapce_width); // _vertical_scroll_bar_width
		// auto size_width = size().width();
		auto show_fields = appconfig->record_table_show_fields();
		auto show_fields_width = appconfig->record_table_fields_width();
		// if(show_fields.contains("row number"))
		// QMutableListIterator<QString> it(show_fields);
		for (auto it : show_fields)
			if (it == "row number")
				show_fields.removeOne(it);
		int required_width = row_number_width;
		// int	i		= 0;
		for (auto w : show_fields_width)
			required_width += w.toInt(); // columnWidth(i ++);
		for (int i = 0; i < show_fields.size(); i++) {
			if (required_width >= real_capacity) { // if((columnWidth(0) +
							       // columnWidth(1)) >= real_width){
				if (is_field_type_column(boost::mpl::c_str<pin_key>::value, i))
					setColumnWidth(i, _pin_width);
				else if (is_field_type_column(boost::mpl::c_str<rating_key>::value, i))
					setColumnWidth(i, rating_width());
				else if (!is_field_type_column(boost::mpl::c_str<name_key>::value, i))
					suggest_others_width += columnWidth(i);
				// if(_is_field_type_column(boost::mpl::c_str<name_type>::value, i)){
				// setColumnWidth(i, (real_capacity >= suggest_others_width) ?
				// real_capacity - suggest_others_width : columnWidth(i));
				////		    horizontalHeader()->setSectionResizeMode(i,
				///QHeaderView::Stretch);
				// }
			} else {
				if (is_field_type_column(boost::mpl::c_str<pin_key>::value, i))
					setColumnWidth(i, _pin_width);
				else if (is_field_type_column(boost::mpl::c_str<rating_key>::value, i))
					setColumnWidth(i, rating_width());
				else if (!is_field_type_column(boost::mpl::c_str<name_key>::value, i))
					suggest_others_width += columnWidth(i);
				// real_capacity = this->contentsRect().width();
				////            if(real_capacity >= 300){
				////
				///if(_is_field_type_column(boost::mpl::c_str<name_type>::value,
				///i))setColumnWidth(i, 300 - adjust_width);					//
				///restoreColumnWidth();
				////            }else{
				// if(_is_field_type_column(boost::mpl::c_str<name_type>::value, i)){
				// setColumnWidth(i, (real_capacity >= suggest_others_width) ?
				// real_capacity - suggest_others_width : columnWidth(i));
				////		    horizontalHeader()->setSectionResizeMode(i,
				///QHeaderView::Stretch);
				// }
				////            }
			}
		}
		for (int i = 0; i < show_fields.size(); i++) {
			if (required_width >= real_capacity) {
				if (is_field_type_column(boost::mpl::c_str<name_key>::value, i)) {
					setColumnWidth(i, (real_capacity >= suggest_others_width) ? real_capacity - suggest_others_width : columnWidth(i));
					// horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
				}
			} else {
				real_capacity = this->contentsRect().width();
				if (is_field_type_column(boost::mpl::c_str<name_key>::value, i)) {
					setColumnWidth(i, (real_capacity >= suggest_others_width) ? real_capacity - suggest_others_width : columnWidth(i));
					// horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
				}
			}
		}
		// for(int j = 0; j < horizontalHeader()->count(); j ++)
		// if(_is_field_type_column(boost::mpl::c_str<name_type>::value,
		// j))horizontalHeader()->setSectionResizeMode(j, QHeaderView::Stretch);
	}
	QTableView::resizeEvent(e);
}


VerticalScrollArea* rv_t::vertical_scrollarea() const { return _vertical_scrollarea; }


template <>
pos_proxy rv_t::selection_first<pos_proxy>() const
{
	// Получение списка выделенных Item-элементов
	QModelIndexList selectItems = selectionModel()->selectedIndexes();
	if (selectItems.isEmpty())
		return pos_proxy(-1); // Если ничего не выделено
	else
		return pos_proxy((selectItems.at(0)).row()); // Номер первого выделенного элемента
}

template <>
pos_source rv_t::selection_first<pos_source>() const
{
	pos_proxy pos_proxy_ = selection_first<pos_proxy>();
	//	auto _rctrl = _rctrl;
	if (_rctrl) {
		return _rctrl->index<pos_source>(pos_proxy_);
	} else
		return pos_source(-1);
}

template <>
id_value rv_t::selection_first<id_value>() const
{
	// Получение списка выделенных Item-элементов
	QModelIndexList selectItems = selectionModel()->selectedIndexes();
	if (selectItems.isEmpty())
		return id_value(""); // Если ничего не выделено

	return id_value(selectItems.at(0).data(RECORD_ID_ROLE).toString());
}

template <>
index_proxy rv_t::selection_first<index_proxy>() const
{
	auto result = index_proxy(QModelIndex());

	pos_proxy pos_proxy_ = selection_first<pos_proxy>();
	if (pos_proxy_ == -1)
		result = index_proxy(QModelIndex());
	else {
		// QModelIndex index = recordProxyModel->index( pos, 0 );
		//		auto _rctrl = _rctrl;
		if (_rctrl) {
			result = _rctrl->index<index_proxy>(pos_proxy(pos_proxy_));
		}
	}
	return result;
}

template <>
index_source rv_t::selection_first<index_source>() const
{
	index_source index;
	index_proxy proxy_index = selection_first<index_proxy>();
	if (!((QModelIndex)proxy_index).isValid())
		index = index_source(QModelIndex());
	else {
		// QModelIndex index = recordProxyModel->mapToSource( proxyIndex );
		//		auto _rctrl = _rctrl;
		if (_rctrl) {
			index = _rctrl->index<index_source>(proxy_index);
		}
	}
	return index;
}

template <>
boost::intrusive_ptr<i_t>
rv_t::selection_first<boost::intrusive_ptr<i_t>>() const
{
	boost::intrusive_ptr<i_t> result;
	//	auto _rctrl = _rctrl;
	if (_rctrl) {
		result = _rctrl->source_model()->item(selection_first<pos_source>());
	}
	return result;
}
