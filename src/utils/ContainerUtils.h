#pragma once
#include <vector>


namespace md::utils {
    template <typename T>
    class FixedVector {
    public:
        // void emplace_back(T value) {container.emplace_back(value);}
        size_t size() const { return container.size(); }
        void lock();

    private:
        std::vector<T> container{};
        bool locked = false;
    };



    template <typename Container>
    class PairIterator final {
    public:
        using ContainerIt = typename Container::iterator;
        using ResetFunction = std::function<void(Container&, Container&, ContainerIt&, ContainerIt&)>;
        using value_type = std::pair<typename Container::value_type, typename Container::value_type>;
        using reference = std::pair<const typename Container::value_type&, const typename Container::value_type&>;
        using iterator_category = std::forward_iterator_tag;

        PairIterator& operator++() {
            ++inner;
            if (inner == inner_container.end()) {
                increment_outer(inner_container, outer_container, inner, outer);
            }
            return *this;
        }

        PairIterator operator++(int) {
            PairIterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const PairIterator& other) const {
            return inner == other.inner && outer == other.outer;
        }

        bool operator!=(const PairIterator& other) const {
            return !(*this == other);
        }

        reference operator*() const {
            return {*outer, *inner};
        }

        reference operator*() {
            return {*outer, *inner};
        }


        static PairIterator CrossPairIterator(Container& inner_cont, Container& outer_cont) {
            using iter = typename Container::iterator;
            auto func = [](Container&cont1, Container&cont2, iter &inner, iter&outer) {
                ++outer;
                if (outer != cont2.end()) {
                    inner = cont1.begin();
                }
            };

            PairIterator it (inner_cont, outer_cont, func);
            if (inner_cont.empty() || outer_cont.empty()) {
                it.inner = inner_cont.end();
                it.outer = outer_cont.end();
            }

            return it;
        }


        static PairIterator UniquePairIterator(Container& container) {
            using iter = typename Container::iterator;
            auto func = [](Container&cont1, Container&cont2, iter&inner, iter&outer) {
                ++outer;
                if (std::next(outer) == cont2.end()) {
                    inner = cont1.end();
                    outer = cont2.end();
                } else {
                    inner = std::next(outer);
                }
            };


            PairIterator it (container, container, func);
            if (container.size() <= 1) {
                it.inner = container.end();
                it.outer = container.end();
            } else {
                ++it.inner;
            }

            return it;
        }


        static PairIterator end(Container& inner_cont, Container& outer_cont) {
            PairIterator it (inner_cont, outer_cont, {});
            it.inner = inner_cont.end();
            it.outer = outer_cont.end();
            return it;
        }


    private:
        PairIterator(Container& inner_container, Container& outer_container, ResetFunction increment_outer)
            : inner(inner_container.begin()), outer(outer_container.begin()),
              inner_container(inner_container), outer_container(outer_container), increment_outer(increment_outer) {}

        ContainerIt inner;
        ContainerIt outer;
        Container& inner_container;
        Container& outer_container;

        ResetFunction increment_outer{};
    };

    template <typename Container>
    class DualPairIterator final {
    public:
        DualPairIterator(Container& inner_container, Container& outer_container):
        inner_container(inner_container), outer_container(outer_container) {}

        PairIterator<Container> begin() {
            if (inner_container == outer_container) {
                return PairIterator<Container>::UniquePairIterator(inner_container);
            } else {
                return PairIterator<Container>::CrossPairIterator(inner_container, outer_container);
            }
        }

        PairIterator<Container> end() {
            return PairIterator<Container>::end(inner_container, outer_container);
        }
    private:
        Container& inner_container;
        Container& outer_container;
    };
}
