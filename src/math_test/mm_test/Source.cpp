#define _CRT_SECURE_NO_WARNINGS 1

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/IterativeLinearSolvers>
#include <unsupported/Eigen/IterativeSolvers>

class MatrixReplacement;
using Eigen::SparseMatrix;

namespace Eigen {
    namespace internal {
        // MatrixReplacement looks-like a SparseMatrix, so let's inherits its traits:
        template<>
        struct traits<MatrixReplacement> : public Eigen::internal::traits<Eigen::SparseMatrix<double> >
        {};
    }
}

// Example of a matrix-free wrapper from a user type to Eigen's compatible type
// For the sake of simplicity, this example simply wrap a Eigen::SparseMatrix.
class MatrixReplacement : public Eigen::EigenBase<MatrixReplacement> {
public:
    // Required typedefs, constants, and method:
    typedef double Scalar;
    typedef double RealScalar;
    typedef int StorageIndex;
    enum {
        ColsAtCompileTime = Eigen::Dynamic,
        MaxColsAtCompileTime = Eigen::Dynamic,
        IsRowMajor = false
    };

    Index rows() const { return mp_mat->rows(); }
    Index cols() const { return mp_mat->cols(); }

    template<typename Rhs>
    Eigen::Product<MatrixReplacement, Rhs, Eigen::AliasFreeProduct> operator*(const Eigen::MatrixBase<Rhs>& x) const {
        return Eigen::Product<MatrixReplacement, Rhs, Eigen::AliasFreeProduct>(*this, x.derived());
    }

    // Custom API:
    MatrixReplacement() : mp_mat(0) {}

    void attachMyMatrix(const SparseMatrix<double>& mat) {
        mp_mat = &mat;
    }
    const SparseMatrix<double> my_matrix() const { return *mp_mat; }

private:
    const SparseMatrix<double>* mp_mat;
};


// Implementation of MatrixReplacement * Eigen::DenseVector though a specialization of internal::generic_product_impl:
namespace Eigen {
    namespace internal {

        template<typename Rhs>
        struct generic_product_impl<MatrixReplacement, Rhs, SparseShape, DenseShape, GemvProduct> // GEMV stands for matrix-vector
            : generic_product_impl_base<MatrixReplacement, Rhs, generic_product_impl<MatrixReplacement, Rhs> >
        {
            typedef typename Product<MatrixReplacement, Rhs>::Scalar Scalar;

            template<typename Dest>
            static void scaleAndAddTo(Dest& dst, const MatrixReplacement& lhs, const Rhs& rhs, const Scalar& alpha)
            {
                // This method should implement "dst += alpha * lhs * rhs" inplace,
                // however, for iterative solvers, alpha is always equal to 1, so let's not bother about it.
                assert(alpha == Scalar(1) && "scaling is not implemented");
                EIGEN_ONLY_USED_FOR_DEBUG(alpha);

                // Here we could simply call dst.noalias() += lhs.my_matrix() * rhs,
                // but let's do something fancier (and less efficient):
                for (Index i = 0; i < lhs.cols(); ++i)
                    dst += rhs(i) * lhs.my_matrix().col(i);
            }
        };

    }
}

//https://habr.com/ru/post/271723/
int main()
{
    int nn = 4;
    Eigen::setNbThreads(nn);


    //int n = 10;
    //Eigen::SparseMatrix<double> S = Eigen::MatrixXd::Random(n, n).sparseView(0.5, 1);
    //S = S.transpose() * S;

    int N = 4;
    int M = 4;
    Eigen::SparseMatrix<double> matrix1(N, M);
    matrix1.reserve(Eigen::VectorXi::Constant(M, 4)); // 4: estimated number of non-zero enties per column
    matrix1.coeffRef(0, 0) = 1;
    matrix1.coeffRef(0, 1) = 2.;
    matrix1.coeffRef(1, 1) = 3.;
    matrix1.coeffRef(2, 2) = 4.;
    matrix1.coeffRef(2, 3) = 5.;
    matrix1.coeffRef(3, 2) = 6.;
    matrix1.coeffRef(3, 3) = 7.;
    matrix1.makeCompressed();

    MatrixReplacement A;
    A.attachMyMatrix(matrix1);




    Eigen::VectorXd b(N), x;
    //b.setRandom();
    b << 1, 2, 3, 4;

    // Solve Ax = b using various iterative solver with matrix-free version:
    {
        Eigen::ConjugateGradient<MatrixReplacement, Eigen::Lower | Eigen::Upper, Eigen::IdentityPreconditioner> cg;
        cg.compute(A);
        x = cg.solve(b);
        std::cout << "CG:       #iterations: " << cg.iterations() << ", estimated error: " << cg.error() << std::endl;
        std::cout << x << std::endl;
    }

    {
        Eigen::BiCGSTAB<MatrixReplacement, Eigen::IdentityPreconditioner> bicg;
        bicg.compute(A);
        x = bicg.solve(b);
        std::cout << "BiCGSTAB: #iterations: " << bicg.iterations() << ", estimated error: " << bicg.error() << std::endl;
        std::cout << x << std::endl;
    }

    {
        Eigen::GMRES<MatrixReplacement, Eigen::IdentityPreconditioner> gmres;
        gmres.compute(A);
        x = gmres.solve(b);
        std::cout << "GMRES:    #iterations: " << gmres.iterations() << ", estimated error: " << gmres.error() << std::endl;
        std::cout << x << std::endl;
    }

    {
        Eigen::DGMRES<MatrixReplacement, Eigen::IdentityPreconditioner> gmres;
        gmres.compute(A);
        x = gmres.solve(b);
        std::cout << "DGMRES:   #iterations: " << gmres.iterations() << ", estimated error: " << gmres.error() << std::endl;
        std::cout << x << std::endl;
    }

    {
        Eigen::MINRES<MatrixReplacement, Eigen::Lower | Eigen::Upper, Eigen::IdentityPreconditioner> minres;
        minres.compute(A);
        x = minres.solve(b);
        std::cout << "MINRES:   #iterations: " << minres.iterations() << ", estimated error: " << minres.error() << std::endl;
        std::cout << x << std::endl;
    }

    //Eigen::SimplicialCholesky<SpMat> chol(A);  // performs a Cholesky factorization of A

    {
        //https://eigen.tuxfamily.org/dox/group__TutorialLinearAlgebra.html
        Eigen::Matrix3f A;
        Eigen::Vector3f b;
        A << 1, 2, 3, 4, 5, 6, 7, 8, 10;
        b << 3, 3, 4;
        std::cout << "Here is the matrix A:\n" << A << std::endl;
        std::cout << "Here is the vector b:\n" << b << std::endl;
        Eigen::Vector3f x = A.colPivHouseholderQr().solve(b);
        std::cout << "The solution is:\n" << x << std::endl;
    }



    /*
    {
        Eigen::MatrixXd MtxA = Eigen::MatrixXd::Random(4, 4);
        MtxA << 1, 2, 0, 1, 3, 4, 0, 0, 0, 0, 2, 3, 1, 0, 4, 5;

        Eigen::VectorXd b(4); // the rbight hand side-vector
        b << 9, 11, 18, 33; //this is RHS for solution vector x=1,2,3,4

        //-------------------------------------------------------
        //   a) solve using preconditioned BICGSTAB method
        //-------------------------------------------------------
        Eigen::BiCGSTAB<SparseMatrix<double>, Eigen::IncompleteLUT<double>>  BCGSTCOND;
        BCGSTCOND.preconditioner().setDroptol(.001);
        BCGSTCOND.compute(SparseA);
        Eigen::VectorXd X;
        X = BCGSTCOND.solve(b);
        std::cout << "Here is the Vector x using BICGSTAB Preconditioned:\n" << X << endl;
      

        //-------------------------------------------------------
        //   b) solve using non-preconditioned BICGSTAB method
        //-------------------------------------------------------
        Eigen::BiCGSTAB<SparseMatrix<double> >  BCGST;
        BCGST.compute(SparseA);
        Eigen::VectorXd Xx;
        Xx = BCGST.solve(b);
        std::cout << "Here is the Vector x using BICGSTAB :\n" << Xx << endl;
    }

    */




}



/*
#include "sparse_solver.h"
#include <Eigen/IterativeLinearSolvers>

template<typename T, typename I_> void test_bicgstab_T()
{
    BiCGSTAB<SparseMatrix<T, 0, I_>, DiagonalPreconditioner<T> >     bicgstab_colmajor_diag;
    BiCGSTAB<SparseMatrix<T, 0, I_>, IdentityPreconditioner    >     bicgstab_colmajor_I;
    BiCGSTAB<SparseMatrix<T, 0, I_>, IncompleteLUT<T, I_> >              bicgstab_colmajor_ilut;
    //BiCGSTAB<SparseMatrix<T>, SSORPreconditioner<T> >     bicgstab_colmajor_ssor;

    bicgstab_colmajor_diag.setTolerance(NumTraits<T>::epsilon() * 4);
    bicgstab_colmajor_ilut.setTolerance(NumTraits<T>::epsilon() * 4);

    CALL_SUBTEST(check_sparse_square_solving(bicgstab_colmajor_diag));
    //   CALL_SUBTEST( check_sparse_square_solving(bicgstab_colmajor_I)     );
    CALL_SUBTEST(check_sparse_square_solving(bicgstab_colmajor_ilut));
    //CALL_SUBTEST( check_sparse_square_solving(bicgstab_colmajor_ssor)     );





    //мое
    BiCGSTAB<SparseMatrix<double, 0, double>, DiagonalPreconditioner<double> >  matrix1;

}

EIGEN_DECLARE_TEST(bicgstab)
{
    CALL_SUBTEST_1((test_bicgstab_T<double, int>()));
    CALL_SUBTEST_2((test_bicgstab_T<std::complex<double>, int>()));
    CALL_SUBTEST_3((test_bicgstab_T<double, long int>()));
}
*/