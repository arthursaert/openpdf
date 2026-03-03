/****************************************************************************
** Meta object code from reading C++ file 'PDFPageWidget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/PDFPageWidget.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PDFPageWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN13PDFPageWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto PDFPageWidget::qt_create_metaobjectdata<qt_meta_tag_ZN13PDFPageWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PDFPageWidget",
        "annotationCreated",
        "",
        "Annotation*",
        "annotation",
        "annotationClicked",
        "annotationRemoved",
        "textSelected",
        "pageNumber",
        "QRectF",
        "normalizedRect"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'annotationCreated'
        QtMocHelpers::SignalData<void(Annotation *)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'annotationClicked'
        QtMocHelpers::SignalData<void(Annotation *)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'annotationRemoved'
        QtMocHelpers::SignalData<void(Annotation *)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'textSelected'
        QtMocHelpers::SignalData<void(int, QRectF)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 }, { 0x80000000 | 9, 10 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PDFPageWidget, qt_meta_tag_ZN13PDFPageWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PDFPageWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13PDFPageWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13PDFPageWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13PDFPageWidgetE_t>.metaTypes,
    nullptr
} };

void PDFPageWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PDFPageWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->annotationCreated((*reinterpret_cast<std::add_pointer_t<Annotation*>>(_a[1]))); break;
        case 1: _t->annotationClicked((*reinterpret_cast<std::add_pointer_t<Annotation*>>(_a[1]))); break;
        case 2: _t->annotationRemoved((*reinterpret_cast<std::add_pointer_t<Annotation*>>(_a[1]))); break;
        case 3: _t->textSelected((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QRectF>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (PDFPageWidget::*)(Annotation * )>(_a, &PDFPageWidget::annotationCreated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (PDFPageWidget::*)(Annotation * )>(_a, &PDFPageWidget::annotationClicked, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (PDFPageWidget::*)(Annotation * )>(_a, &PDFPageWidget::annotationRemoved, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (PDFPageWidget::*)(int , QRectF )>(_a, &PDFPageWidget::textSelected, 3))
            return;
    }
}

const QMetaObject *PDFPageWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PDFPageWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13PDFPageWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int PDFPageWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void PDFPageWidget::annotationCreated(Annotation * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void PDFPageWidget::annotationClicked(Annotation * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void PDFPageWidget::annotationRemoved(Annotation * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void PDFPageWidget::textSelected(int _t1, QRectF _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}
QT_WARNING_POP
