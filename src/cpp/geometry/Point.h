#ifndef PXSORT_POINT_H
#define PXSORT_POINT_H

#include "fwd.h"
#include "util.h"
#include <Eigen/Core>

namespace pxsort {

    /**
     * Extension of Eigen's 2d Vector adding extra functions useful for this
     * library.
     * @tparam Scalar
     */
    template <typename Scalar>
    class Point_ : public Eigen::Matrix<Scalar, 2, 1> {
    public:
        using Eigen::Matrix<Scalar, 2, 1>::Matrix;

        [[nodiscard]]
        Point_<Scalar> constrain(const Point_<Scalar> &lo,
                                 const Point_<Scalar> &hi) const {
            return {clamp(this->x(), lo.x(), hi.x()),
                    clamp(this->y(), lo.y(), hi.y())};
        }

        /** Explicit typecasting to other arithmetic types */
        template<typename S> requires Arithmetic<S>
        explicit operator Point_<S>() const {
            return {static_cast<S>(this->x()), static_cast<S>(this->y())};
        }

        Point_<Scalar> operator%(const Point_<Scalar> &that) const;

        Point_<Scalar> operator-(const Point_<Scalar> &that) const {
            return {this->x() - that.x(), this->y() - that.y()};
        }

        Point_<Scalar> operator+(const Point_<Scalar> &that) const {
            return {this->x() + that.x(), this->y() + that.y()};
        }
    };

    using Point = Point_<int32_t>;
    using Point2f = Point_<float>;


    template<>
    inline Point Point::operator%(const Point &that) const {
        return {modulo(this->x(), that.x()),
                modulo(this->y(), that.y())};
    }

    template<>
    inline Point2f Point2f::operator%(const Point2f &that) const {
        return {std::fmod(this->x(), that.x()),
                std::fmod(this->y(), that.y())};
    }
}

template <>
struct std::hash<pxsort::Point> {
    std::size_t operator()(const pxsort::Point& pt) const noexcept {
        int64_t const x = pt.x();
        int64_t const y = pt.y();
        return std::hash<uint64_t>()(x | (y << 32));
    }
};

#endif //PXSORT_POINT_H
